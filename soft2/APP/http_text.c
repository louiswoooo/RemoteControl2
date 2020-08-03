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

