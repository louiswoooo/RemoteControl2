#include "http_ap.h"

u8 const PORTAL_PAGE[]	=	"HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
Content-Length:152\r\n\
\r\n\
<!DOCTYPE html>\r\n\
<html lang=\"en\">\r\n\
<head>\r\n\
    <meta charset=\"UTF-8\">\r\n\
    <title>���</title>\r\n\
</head>\r\n\
<body>\r\n\
    <h1>�������</h1>\r\n\
</body>\r\n\
</html>";




/*
u8 const WIFI_PAGE[]	=	"HTTP/1.1 200 OK
Content-Type: text/html
Content-Length:361

<html>
<body>
<h1>��ӭ��ʹ����ŵ�AIʵ���豸</h1>
<p>��������WIFI���ƺ����룬ͨ����WIFI���豸�������ӵ�����ơ�</p>
<form action=\"/wifiset.html\">
WIFI����:<br>
<input type=\"text\" name=\"firstname\" value=\"Louiswoo\">
<br>
����:<br>
<input type=\"text\" name=\"lastname\" value=\"Genius\">
<br><br>
<input type=\"submit\" value=\"����\">
</form>
</body>
</html>";

u8 const CONNECTED_PAGE[]	=	"HTTP/1.1 200 OK
Content-Type: text/html
Content-Length:77

<html>
<body>
<h1>WIFI�ɹ����ӣ���л��ʹ������ƣ�</h1>
</body>
</html>";

u8 const PORTAL_PAGE[]	=	"<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>���AIʵ��ƽ̨</title>
    <style>
        h1{font-size: 600%;text-align: center}
        .wd{font-size: 300%;margin-left: 40px}
        .sw1,.sw2{margin-left: 40px;width: 120px;height: 120px;background: #ff9953;border-radius: 60px;padding: 20px}
        .sw2{background: darkgray}
        .l1,.l2,.l3,.l4{margin-left: 40px;width: 120px;height: 120px;background: #ff9953;border-radius: 30px;padding: 20px}
        .l2{background: #ffd689}
        .l3{background: #d8c3ed}
        .l4{background: darkgray}

    </style>
</head>
<body>
    <div id="container" style="width: 1000px">
        <div id="header" style="background-color: slategray">
            <h1 style="margin-bottom:0;">���AIʵ��ƽ̨</h1>
        </div>
        <div id="content" style="background-color: #EEEEEE;height: 1200px;width: 1000px;float: left;">
        <br>
            <form name="sw" action="table" method="get">
                <span class="wd">1�ſ���:</span>
                <button class="sw1" type="submit" name="SWITCH1" value="ON">��</button>
                <button class="sw2" type="submit" name="SWITCH1" value="OFF">��</button>
                <br>
                <span class="wd">2�ſ���:</span>
                <button class="sw1" type="submit" name="SWITCH2" value="ON">��</button>
                <button class="sw2" type="submit" name="SWITCH2" value="OFF">��</button><br>
                <span class="wd">3�ſ���:</span>
                <button class="sw1" type="submit" name="SWITCH3" value="ON">��</button>
                <button class="sw2" type="submit" name="SWITCH3" value="OFF">��</button><br>
                <span class="wd">4�ſ���:</span>
                <button class="sw1" type="submit" name="SWITCH4" value="ON">��</button>
                <button class="sw2" type="submit" name="SWITCH4" value="OFF">��</button>
            </form>
            <br>
            <hr>
        </div>
    </div>
    <br>
</body>
</html>";
*/
