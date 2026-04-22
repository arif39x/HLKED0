<!DOCTYPE HTML>
<html>
<head>
<meta charset="utf-8">
<title>Unbenanntes Dokument</title>
</head>
<body>
//hwid, socksip, socksport, httpport, osversion, antivir, version
<form method="post" action="exec.php?mode=con">
hwid: <input type="text" name="hwid" /><br />
socksip: <input type="text" name="socksip" /><br />
socksport: <input type="text" name="socksport" /><br />
httpport: <input type="text" name="httpport" /><br />
osversion: <input type="text" name="osversion" /><br />
antivir: <input type="text" name="antivir" /><br />
version: <input type="text" name="version" /><br />
<input type="submit" />
</form>

//socksport, httpport
<form method="post" action="exec.php?mode=proxy">
hwid: <input type="text" name="hwid" /><br />
socksport: <input type="text" name="socksport" /><br />
httpport: <input type="text" name="httpport" /><br />
<input type="submit" />
</form>
</body>
</html>