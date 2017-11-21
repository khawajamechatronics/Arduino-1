join packet service AT+CGATT=0
leaving AT+CGATT=1 also gives unsolicited 
          +CTZV:17/01/08,21:23:05,+02
          +CGREG: 1
set context
AT+CGDCONT=1,"IP","uinternet"
activate context
AT+CGACT=1,1  replies with 

+CTZV:17/01/08,21:33:19,+02
+CGREG: 1
OK
+CGREG: 1
      
get local IP address
AT+CIFSR

connect to TCP server
AT+CIPSTART="TCP","david-henry.dyndns.tv",80
close wireless connection 
AT+CIPSHUT
start wireless task
AT+CSTT="uinternet","",""
bring up wireless conection
AT+CIICR

query ip address
AT+CDNSGIP="david-henry.dyndns.tv"

AT+CGATT=1
AT+CGDCONT=1,"IP","uinternet"
AT+CGACT=1,1
AT+CIPSTART="TCP","david-henry.dyndns.tv",80
AT+CIPSEND
GET /  HTTP/1.0
Host: "david-henry.dyndns.tv"
User-Agent: Arduino



after sending ctrl-z got

 
                                                                            
OK
+CIPRCV:1200,HTTP/1.1 200 OK
Date: Tue, 10 Jan 2017 14:15:44 GMT
Server: Apache/2.4.10 (Raspbian)
Last-Modified: Wed, 20 Jan 2016 15:39:58 GMT
ETag: "39f-529c5cc64af80"
Accept-Ranges: bytes
Content-Length: 927
Vary: Accept-Encoding
Connection: close
Content-Type: text/html
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<title>David Henry Home Page</title><meta name="genera
tor" content="Bluefish 1.0.7"><meta name="author" content="tinyos">
<meta name="date" content="2008-10-04T23:48:26+0300">
<meta name="copyright" content="">
<meta name="keywords" content="">
<meta name="description" content="">
<!-- <meta name="ROBOTS" content="NOINDEX, NOFOLLOW"> -->
<meta http-equiv="content-type" content="text/html; charset=UTF-8">
<meta http-equiv="content-type" content="application/xhtml+xml; charset=UTF-8">
<meta http-equiv="content-style-type" content="text/css">
<meta >
<base  HREF="http://david-henry.dyndns.tv/" TARGET="_top">
!base HREF="http://10.0.0.6/" TARGET="_top">
</head>
<body>
<h1>Welcome to David Henry's Home Page</h1>
<h2>Current Sites on Offer</h2>
<a href=genealogy/persons.html>My Genealogy Pages</a>
</body>
</html>
                                
                                              
OK
+TCPCLOSED:0
											  
GET /WC/last3.php  HTTP/1.0
Host: "david-henry.dyndns.tv"
User-Agent: Arduino

