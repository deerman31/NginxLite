#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import html

print("Content-Type: text/html")    # HTMLを返すことを宣言
print()                             # ヘッダの終了を示す空行

print("<html>")
print("<head>")
print("<title>CGI Script Test</title>")
print("</head>")
print("<body>")
print("<h1>Environment Variables</h1>")
print("<table border='1'>")
print("<tr><th>Variable</th><th>Value</th></tr>")

for key, value in os.environ.items():
    print(f"<tr><td>{html.escape(key)}</td><td>{html.escape(value)}</td></tr>")

print("</table>")
print("</body>")
print("</html>")
