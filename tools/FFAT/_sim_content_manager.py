"""
Logic-level simulation of the ContentManager/Scroll/Animations changes.
Ports the exact algorithms from src/ContentManager.cpp and src/Animations.cpp
so they can be executed and asserted against, since no ESP32 hardware or
emulator is available to run the real firmware. Not a substitute for an
on-device check -- delete after use, or keep as a regression script.
"""

COLS, ROWS = 20, 25

# ---------------------------------------------------------------------------
# 1) Scroll duration formula (ContentManager.cpp discoverContent, scroll branch)
# ---------------------------------------------------------------------------
def scroll_duration(text: str, speed: int, explicit_duration: int | None) -> int:
    if explicit_duration:
        return explicit_duration
    total_width = len(text) * 6
    duration = (COLS * 2 + total_width + 1) * speed
    return duration or 5000


def scroll_pass_ticks(text: str) -> int:
    # Mirrors Scroll::update(): starts at COLS*2, stops when scrollPos < -totalWidth
    total_width = len(text) * 6
    start = COLS * 2
    ticks = 0
    pos = start
    while pos >= -total_width:
        pos -= 1
        ticks += 1
    return ticks


print("=== Scroll duration ===")
cases = [
    ("Happy Halloween!", 50, None),
    ("Trick or Treat!", 50, None),
    ("Happy New Year 2026!", 45, None),
    ("GO BUCKEYES!", 40, None),
]
for text, speed, explicit in cases:
    dur = scroll_duration(text, speed, explicit)
    ticks = scroll_pass_ticks(text)
    full_pass_ms = ticks * speed
    ok = dur >= full_pass_ms
    print(f"  '{text}': computed durationMs={dur}  full_pass={full_pass_ms}  "
          f"{'OK covers full pass' if ok else 'FAIL - would still cut off'}")
    assert ok, f"scroll_duration undershoots full pass for '{text}'"

old_default = 5000
old_full_pass = scroll_pass_ticks("Happy Halloween!") * 50
print(f"  old flat default=5000ms vs needed={old_full_pass}ms -> "
      f"{'would have cut off' if old_default < old_full_pass else 'fine'} (reproduces the bug)")
assert old_default < old_full_pass, "expected to reproduce the original cutoff bug"


# ---------------------------------------------------------------------------
# 2) updateRandomMode gating (ContentManager.cpp)
# ---------------------------------------------------------------------------
SCENE, ANIMATION, SCROLL, COUNTDOWN, PROCEDURAL = range(5)

class FakeContentManager:
    def __init__(self, random_interval_ms=4000):
        self.random_interval_ms = random_interval_ms
        self.last_random_change = 0
        self.active_type = SCENE
        self.active_item_duration = 0
        self.switch_log = []  # (time, reason)

    def start_content(self, now, ctype, duration_ms):
        self.active_type = ctype
        self.active_item_duration = duration_ms
        self.last_random_change = now
        self.switch_log.append((now, f"start type={ctype} dur={duration_ms}"))

    def update_random_mode(self, now):
        elapsed = now - self.last_random_change
        if elapsed < self.random_interval_ms:
            return False
        if self.active_type in (ANIMATION, SCROLL):
            if elapsed < self.active_item_duration:
                return False  # gated: must finish its own cycle first
        return True  # would call selectRandomContent()


print("\n=== updateRandomMode gating ===")
cm = FakeContentManager(random_interval_ms=4000)
cm.start_content(now=0, ctype=SCROLL, duration_ms=6800)  # Happy Halloween! scroll
# Old bug: interval alone (4000ms) would have switched here.
switched_at_4000 = cm.update_random_mode(4000)
print(f"  scroll running, interval elapsed at t=4000: would_switch={switched_at_4000} "
      f"(must be False, scroll needs 6800ms)")
assert switched_at_4000 is False

switched_at_6799 = cm.update_random_mode(6799)
print(f"  t=6799 (1ms before scroll completes): would_switch={switched_at_6799}")
assert switched_at_6799 is False

switched_at_6800 = cm.update_random_mode(6800)
print(f"  t=6800 (scroll just completed): would_switch={switched_at_6800}")
assert switched_at_6800 is True

# Scenes are not gated by their own duration beyond the interval.
cm2 = FakeContentManager(random_interval_ms=4000)
cm2.start_content(now=0, ctype=SCENE, duration_ms=999999)  # huge "duration", irrelevant for scenes
assert cm2.update_random_mode(4000) is True, "scenes should switch on interval alone"
print("  scene with huge stored duration still switches at interval (4000ms): OK")

# Animation with a loop longer than the interval must also wait.
cm3 = FakeContentManager(random_interval_ms=4000)
cm3.start_content(now=0, ctype=ANIMATION, duration_ms=4700)  # e.g. franky_timeline sum
assert cm3.update_random_mode(4000) is False
assert cm3.update_random_mode(4700) is True
print("  animation (4700ms loop) waits past the 4000ms interval, switches at 4700ms: OK")


# ---------------------------------------------------------------------------
# 3) Random-mode repeat cooldown (selectRandomContent)
# ---------------------------------------------------------------------------
import random as _random

def cooldown_for(pool_size: int) -> int:
    cd = pool_size - 1 if pool_size > 1 else 0
    return min(cd, 4)

def simulate_random_picks(pool_ids, n_picks, seed=1):
    rnd = _random.Random(seed)
    recent = []
    picks = []
    for _ in range(n_picks):
        cd = cooldown_for(len(pool_ids))
        while len(recent) > cd:
            recent.pop(0)
        fresh = [i for i in pool_ids if i not in recent]
        if not fresh:
            fresh = list(pool_ids)  # matches C++ fallback when everything is on cooldown
        choice = rnd.choice(fresh)
        picks.append(choice)
        recent.append(choice)
    return picks

print("\n=== Random-mode repeat cooldown ===")
pool10 = list(range(10))
picks = simulate_random_picks(pool10, 200)
violations = 0
for i in range(len(picks)):
    window = picks[max(0, i - 4):i]
    if picks[i] in window:
        violations += 1
print(f"  pool=10 items, 200 picks: repeats within last-4 window = {violations} (expect 0)")
assert violations == 0

pool3 = list(range(3))  # small pool: cooldown should shrink to 2 (pool-1, capped at 4)
picks3 = simulate_random_picks(pool3, 200)
cd3 = cooldown_for(3)
print(f"  pool=3 items -> cooldown={cd3} (expect 2)")
assert cd3 == 2
violations3 = 0
for i in range(len(picks3)):
    window = picks3[max(0, i - cd3):i]
    if picks3[i] in window:
        violations3 += 1
print(f"  pool=3 items, 200 picks: repeats within last-{cd3} window = {violations3} (expect 0)")
assert violations3 == 0

pool1 = [0]
cd1 = cooldown_for(1)
print(f"  pool=1 item -> cooldown={cd1} (expect 0, only choice is itself)")
assert cd1 == 0


# ---------------------------------------------------------------------------
# 4) colorWave checkerboard load (Animations.cpp colorWave)
# ---------------------------------------------------------------------------
print("\n=== colorWave checkerboard load ===")
def colorwave_lit_count():
    lit = 0
    for x in range(COLS):
        for y in range(ROWS):
            if (y % 2) == (x % 2):
                lit += 1
    return lit

lit = colorwave_lit_count()
total = COLS * ROWS
print(f"  lit per matrix = {lit} / {total} ({lit*100/total:.1f}%), old code lit all {total} (100%)")
assert lit == 250, f"expected exactly half (250), got {lit}"
assert lit < total


print("\nAll simulated checks passed.")
