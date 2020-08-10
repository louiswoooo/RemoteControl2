#include "config.h"
u8  code HTTP_Client_Request_Head1[]={"GET /a11?"};
u8  code HTTP_Client_Request_Head3[]={" HTTP/1.1\r\n\
Host: localhost:5000\r\n\
Connection: keep-alive\r\n\
\r\n"};

u8 code HTTP_Server_Response_Head1[]={"HTTP/1.0 200 OK\r\n\
Content-Type: text/html; charset=utf-8\r\n\
Content-Length: "};

u8 code HTTP_Server_Index[]	=	{"<!DOCTYPE html>\r\n\
<html lang=\"en\">\r\n\
<head>\r\n\
    <meta charset=\"UTF-8\">\r\n\
    <title>Genius AI Lab</title>\r\n\
    <style>\r\n\
        h1{font-size:600%;text-align:center}\r\n\
         a{font-size: 300%;margin: 120px 0px 0px 40px}\r\n\
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
    </div>\r\n\
</body>\r\n\
</html>"};

u8 code HTTP_Server_Config[]	=	{"<!DOCTYPE html>\r\n\
<html lang=\"en\">\r\n\
<head>\r\n\
    <meta charset=\"UTF-8\">\r\n\
    <title>天才AI实验室</title>\r\n\
    <style>\r\n\
        h1{background-color: darkgray;margin: 20px;font-size: 64px;text-align: center;}\r\n\
        h2 {margin: 10px 40px;font-size: 54px;}\r\n\
        body{width:800px;}\r\n\
        .main-frame{float: left;background-color: #d6d6d6;float: left;border: solid 2px black;}\r\n\
        .sub-frame{float: left;border: solid 1px gray;}\r\n\
        .thd-frame{margin: 10px;float: left;border: dashed 1px gray;}\r\n\
        .form-line{margin: 10px;float: left;height: 80px;}\r\n\
        .form-line>p{margin: 0px 40px;float: left;font-size: 48px;width: 200px;line-height: 80px;text-align: right;}\r\n\
        .form-line>div,.form-line>input{float: right;width: 400px;height: 76px;border: solid;font-size: 48px;}\r\n\
    </style>\r\n\
</head>\r\n\
<body>\r\n\
    <div class=\"main-frame\">\r\n\
        <h1>天才AI实验室</h1>\r\n\
        <div class=\"sub-frame\">\r\n\
            <form action=\"#\">\r\n\
                <div class=\"thd-frame\">\r\n\
                    <h2>WiFi</h2>\r\n\
                    <div class=\"form-line\"><p>名称</p><input placeholder=\"输入连接的WiFi\" type=\"text\" name=\"ssid\"></div>\r\n\
                    <div class=\"form-line\"><p>密码</p><input placeholder=\"输入WiFi密码\" type=\"text\" name=\"pwd\"></div>\r\n\
                </div>\r\n\
                <div class=\"thd-frame\">\r\n\
                    <h2>服务器</h2>\r\n\
                    <div class=\"form-line\"><p>IP地址</p><input placeholder=\"119.3.233.56\" type=\"text\" name=\"ip\"></div>\r\n\
                    <div class=\"form-line\"><p>端口</p><input placeholder=\"5000\" type=\"text\" name=\"port\"></div>\r\n\
                </div>\r\n\
                <div class=\"thd-frame\">\r\n\
                    <h2>用户</h2>\r\n\
                    <div class=\"form-line\"><p>用户名</p><input placeholder=\"输入自定义用户名\" type=\"text\" name=\"user\"></div>\r\n\
                    <div class=\"form-line\"><p>密码</p><input placeholder=\"输入自定义密码\" type=\"text\" name=\"upwd\"></div>\r\n\
                </div>\r\n\
                <input style=\"float:right;margin: 10px 40px;width: 300px;height: 66px;font-size: 48px;border-radius: 16px\" type=\"submit\" value=\"设定\">\r\n\
            </form>\r\n\
        <a href=\"/config\">AI远程控制设置</a>\r\n\
        </div>\r\n\
    </div>\r\n\
</body>\r\n\
</html>"};
u8 code HTTP_Server_Res[]	=	{"设定成功"};

