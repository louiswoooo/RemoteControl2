#include "http_ap.h"

u8 code HTTP_H1[]="HTTP/1.0 200 OK\r\n\
Content-Type: text/html; charset=utf-8\r\n\
Content-Length: ";

u8 code HTTP_INDEX_CONTENT[]	=	"<!DOCTYPE html>\r\n\
<html lang=\"en\">\r\n\
<head>\r\n\
    <meta charset=\"UTF-8\">\r\n\
    <title>Genius AI Lab</title>\r\n\
    <style>\r\n\
        h1{font-size:600%;text-align:center}\r\n\
        span{font-size:300%;margin-left:40px}\r\n\
        button{type:button;margin-left:40px;width:120px;height:120px;border-radius:60px;padding:20px}\r\n\
        .ss1{background:#ff9953}\r\n\
        .ss2{background:darkgray}\r\n\
        .ss3{background: #ffe0a0}\r\n\
        .ss4{background: #d6d6d6}\r\n\
        .ss5{border-radius:20px;height:60px}\r\n\
    </style>\r\n\
    <script>\r\n\
        function jp(a)\r\n\
        {window.location.href=\"/?\"+a}\r\n\
    </script>\r\n\
</head>\r\n\
<body>\r\n\
    <div style=\"width:1000px\">\r\n\
        <div style=\"background-color:slategray\">\r\n\
            <h1>Genius AI Lab</h1>\r\n\
        </div>\r\n\
        <br>\r\n\
                <span>SWITCH1:</span>\r\n\
                <button class=\"ss1\" onclick=\"jp('SWITCH1=ON')\">ON</button>\r\n\
                <button class=\"ss2\" onclick=\"jp('SWITCH1=OFF')\">OFF</button>\r\n\
                <br>\r\n\
                <span>SWITCH2:</span>\r\n\
                <button class=\"ss1\" onclick=\"jp('SWITCH2=ON')\">ON</button>\r\n\
                <button class=\"ss2\" onclick=\"jp('SWITCH2=OFF')\">OFF</button><br>\r\n\
                <span>SWITCH3:</span>\r\n\
                <button class=\"ss1\" onclick=\"jp('SWITCH3=ON')\">ON</button>\r\n\
                <button class=\"ss2\" onclick=\"jp('SWITCH3=OFF')\">OFF</button><br>\r\n\
                <span>SWITCH4:</span>\r\n\
                <button class=\"ss1\" onclick=\"jp('SWITCH4=ON')\">ON</button>\r\n\
                <button class=\"ss2\" onclick=\"jp('SWITCH4=OFF')\">OFF</button>\r\n\
            <br>\r\n\
        <br>\r\n\
            <hr>\r\n\
        <br>\r\n\
        <span>LIGHT1:</span>\r\n\
        <button class=\"ss1\" onclick=\"jp('LIGHT1=ON')\">ON</button>\r\n\
        <button class=\"ss3\" onclick=\"jp('LIGHT1=INC')\">ADD</button>\r\n\
        <button class=\"ss4\" onclick=\"jp('LIGHT1=DEC')\">DEC</button>\r\n\
        <button class=\"ss2\" onclick=\"jp('LIGHT1=OFF')\">OFF</button><br>\r\n\
        <span>LIGHT2:</span>\r\n\
        <button class=\"ss1\" onclick=\"jp('LIGHT2=ON')\">ON</button>\r\n\
        <button class=\"ss3\" onclick=\"jp('LIGHT2=INC')\">ADD</button>\r\n\
        <button class=\"ss4\" onclick=\"jp('LIGHT2=ON')\">DEC</button>\r\n\
        <button class=\"ss2\" onclick=\"jp('LIGHT2=DEC')\">OFF</button><br>\r\n\
        <br>\r\n\
            <hr>\r\n\
        <br>\r\n\
        <span>Tempreture: <span id=\"tp\">30 'C</span></span><button class=\"ss5\" onclick=\"jp('REFRESH')\">Refresh</button><br>\r\n\
        <span>Humidity: <span id=\"hu\">70%</span></span><br>\r\n\
    </div>\r\n\
</body>\r\n\
</html>";

/*
u8 code HTTP_INDEX_CONTENT[]	=	"<!DOCTYPE html>\r\n\
<html lang=\"en\">\r\n\
<head>\r\n\
    <meta charset=\"UTF-8\">\r\n\
    <title>Genius AI Lab</title>\r\n\
    <style>\r\n\
        h1{font-size:600%;text-align:center}\r\n\
        .wd{font-size:300%;margin-left:40px}\r\n\
        button{type:submit;margin-left:40px;width:120px;height:120px;border-radius:60px;padding:20px}\r\n\
        .sw1{background:#ff9953;value:ON}\r\n\
        .sw2{background:darkgray;value:OFF}\r\n\
    </style>\r\n\
</head>\r\n\
<body>\r\n\
    <div style=\"width:1000px\">\r\n\
        <div style=\"background-color:slategray\">\r\n\
            <h1>Genius AI Lab</h1>\r\n\
        </div>\r\n\
        <br>\r\n\
            <form name=\"sw\" action=\"table\" method=\"post\">\r\n\
                <span class=\"wd\">SWITCH1:</span>\r\n\
                <button class=\"sw1\" name=\"SWITCH1\">ON</button>\r\n\
                <button class=\"sw2\" name=\"SWITCH1\">OFF</button>\r\n\
                <br>\r\n\
                <span class=\"wd\">SWITCH2:</span>\r\n\
                <button class=\"sw1\" name=\"SWITCH2\">ON</button>\r\n\
                <button class=\"sw2\" name=\"SWITCH2\">OFF</button><br>\r\n\
                <span class=\"wd\">SWITCH3:</span>\r\n\
                <button class=\"sw1\" name=\"SWITCH3\">ON</button>\r\n\
                <button class=\"sw2\" name=\"SWITCH3\">OFF</button><br>\r\n\
                <span class=\"wd\">SWITCH4:</span>\r\n\
                <button class=\"sw1\" name=\"SWITCH4\">ON</button>\r\n\
                <button class=\"sw2\" name=\"SWITCH4\">OFF</button>\r\n\
            </form>\r\n\
            <br>\r\n\
            <hr>\r\n\
    </div>\r\n\
    <br>\r\n\
</body>\r\n\
</html>";




*/
/*
u8 const WIFI_PAGE[]	=	"HTTP/1.1 200 OK
Content-Type: text/html
Content-Length:361

<html>
<body>
<h1>欢迎您使用天才的AI实验设备</h1>
<p>请您输入WIFI名称和密码，通过此WIFI，设备可以连接到天才云。</p>
<form action=\"/wifiset.html\">
WIFI名称:<br>
<input type=\"text\" name=\"firstname\" value=\"Louiswoo\">
<br>
密码:<br>
<input type=\"text\" name=\"lastname\" value=\"Genius\">
<br><br>
<input type=\"submit\" value=\"连接\">
</form>
</body>
</html>";

u8 const CONNECTED_PAGE[]	=	"HTTP/1.1 200 OK
Content-Type: text/html
Content-Length:77

<html>
<body>
<h1>WIFI成功连接，感谢您使用天才云！</h1>
</body>
</html>";

u8 const PORTAL_PAGE[]	=	"HTTP/1.0 200 OK\r\n\
Content-Type: text/html; charset=utf-8\r\n\
Content-Length: 149\r\n\
Server: Werkzeug/1.0.1 Python/3.8.1\r\n\
Date: Tue, 14 Jul 2020 09:12:38 GMT\r\n\
\r\n\
<!DOCTYPE html>\r\n\
<html lang=\"en\">\r\n\
<head>\r\n\
    <meta charset=\"UTF-8\">\r\n\
    <title>Geni</title>\r\n\
</head>\r\n\
<body>\r\n\
    i am a Genius!!!\r\n\
</body>\r\n\
</html>";

*/
