#include "WebPages.h"

String WebPages::buildRootPage() {
    String html;

    html += "<!DOCTYPE html><html><head>";
    html += "<meta charset='utf-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<title>Palombaro Matrix Show</title>";
    html += "<style>";
    html += "body{font-family:Arial;background:#111;color:#eee;text-align:center;}";
    html += "h1{margin-top:20px;}";
    html += "button{padding:12px 24px;margin:8px;font-size:16px;}";
    html += "</style>";
    html += "</head><body>";

    html += "<h1>PalombaroMatrix Show</h1>";
    html += "<p>Web UI Online</p>";

    html += "<button>Theme</button>";
    html += "<button>Scene</button>";
    html += "<button>Animation</button>";
    html += "<button>Brightness</button>";

    html += "</body></html>";
    return html;
}
