#ifndef _COMMAND_H_
#define _COMMAND_H_

#define INDEX_KEYWORD		"GET / " 
#define WIFISET_KEYWORD	"GET /wifiset.html" 

#define INDEX_PAGE		"HTTP/1.1 200 OK
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
</html>"

#define CONNECTED_PAGE		"HTTP/1.1 200 OK
Content-Type: text/html
Content-Length:77

<html>
<body>
<h1>WIFI�ɹ����ӣ���л��ʹ������ƣ�</h1>
</body>
</html>"

#endif