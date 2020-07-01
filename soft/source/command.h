#ifndef _COMMAND_H_
#define _COMMAND_H_

#define INDEX_KEYWORD		"GET / " 
#define WIFISET_KEYWORD	"GET /wifiset.html" 

#define INDEX_PAGE		"HTTP/1.1 200 OK
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
</html>"

#define CONNECTED_PAGE		"HTTP/1.1 200 OK
Content-Type: text/html
Content-Length:77

<html>
<body>
<h1>WIFI成功连接，感谢您使用天才云！</h1>
</body>
</html>"

#endif