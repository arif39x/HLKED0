<?php

function ansiencript($source)
{
	for ($i=0; $i < strlen($source); $i++) 
	{ 
		$resul .= chr(ord(substr($source,$i,1))+3);
	}
	return $resul;
}

function ansidecript($source)
{
	for ($i = 0; $i < strlen($source); $i++)
	{
		$result .= chr(ord(substr($source,$i,1)) -3);
	}
	return $result;
}

function comuserpass($user, $pass)
{
	$_user = ansidecript($user);
	$_pass = ansidecript($pass);
	$todo = $_user . $_pass;
	return $todo;
}

function verifica($text1, $archivo)
{
	$lineas = array();
	$lineas = file($archivo, FILE_IGNORE_NEW_LINES);
	$encontrado = 0;
	$cachos = array();
	for ($i = 0; $i < count($lineas); $i++)
	{
		$cachos = explode("#", $lineas[$i]); //se pueden poner comentarios con #
		if (trim($cachos[0]) == trim($text1))
		{
			$encontrado = 1;
		}
	}
	return $encontrado;
}

function loguea($texto)
{
	$handle = fopen("log442.txt","ab");
	date_default_timezone_set("Europe/Madrid");
	$d = date("d-m-Y H:i:s");
	$string = $d . " -> " . $texto . "\n";
	fwrite($handle,$string);
	fclose($handle);	
}

try
{
if ($_GET["ID"] == "login") //verificamos user y password
{
	$_user = $_GET["user"];
	$_pass = $_GET["pass"];
	if ($_user == "" || $_pass == "")
	{
		echo "fuckyou";
		exit;
	}
	$todo = comuserpass($_user,$_pass);
	$a = verifica($todo, "theusers328.txt");
	if ($a == 1)
	{
		echo ansiencript('Correcto');
		//echo "Correcto";
		//loguea($todo);
	} else {
		echo "Error";
		exit;
	}
} else if ($_GET["ID"] == "create") //verificamos hwid
{
	$_hwid = $_GET["hwid"];
	$_version = $_GET["ver"];
	$todo = ansidecript($_hwid);
	$a = verifica($todo, "thehwids936.txt");
	if ($a == 1)
	{
		if ($_version == 3.0)
		{
			echo ansiencript("mythisreleasepassword");
		
		}
	} else {
		echo "cagada";
		exit;
	}
} else if ($_GET["ID"] == update)
{
	$_user = $_GET["user"];
	$_pass = $_GET["pass"];

	if ($_user == "" || $_pass == "")
	{
		echo "fuckyou";
		exit;
	}
	$todo = comuserpass($_user,$_pass);
	$a = verifica($todo, "theusers328.txt");
	if ($a == 1)
	{
		$handle = fopen("actualizados.txt","a+");
		$string = $_SERVER['REMOTE_ADDR'] . "   " . $todo . "\n";
		fwrite($handle,$string);
		fclose($handle);
		$enlace = "update.exe"; //this is de new update monster
		header ("Content-Disposition: attachment; filename=monsterupdate.exe \n\n");
		header ("Content-Type: application/octet-stream");
		header ("Content-Length: ".filesize($enlace));
		readfile($enlace);
	} else {
		echo "Error";
		exit;
	}
}
else if ($_GET["add"] == 1)
{
	if ($_GET["user"] == "" || $_GET["hwid"] == "" || $_GET["id"] == "")
	{
		echo "ERROR";
		exit;
	} else
		$ccc = array();
		$u = array();
		$ccc = file("theusers328.txt", FILE_IGNORE_NEW_LINES);
		for ($i=0; $i < count($ccc); $i++) { 
			$u = explode("#", $ccc[$i]);
			if ($u[1] == $_GET["id"]) {
				echo "Duplicado";
				exit();
			}
		}
		$comentario = $_GET["com"];
		$idunico = trim($_GET["id"]);
		if (!$archivo = fopen('theusers328.txt','a'))
		{
			echo "Error al abrir archivo users";
			exit;
		}
		fwrite($archivo, $_GET["user"] . "#" . $idunico . "#" . $comentario . "\n");
		fclose($archivo);
		if (!$archivo = fopen('thehwids936.txt','a'))
		{
			echo "Error al abrir archivo hwides";
			exit;
		}
		fwrite($archivo, $_GET["hwid"] . "#" . $idunico . "#" . $comentario . "\n");
		fclose($archivo);
		echo "OK";
		exit;
}

 else if ($_GET["list"] == 1)
{
	$users = array();
	$hwids = array();
	$users = file("theusers328.txt", FILE_IGNORE_NEW_LINES);
	$hwids = file("thehwids936.txt", FILE_IGNORE_NEW_LINES);
	$u = array();
	$h = array();
	$idunico = "";
	for ($i = 0; $i < count($users); $i++)
	{
		$u = explode("#", $users[$i]);
		$comentario = $u[2];
		if ($comentario == "")
		{
			$comentario = "Sin comentario";
		}
		$idunico = $u[1];
		for ($x=0; $x < count($hwids); $x++) { 
			$h = explode("#", $hwids[$x]);
			if ($h[1] == $idunico) {
				break;
			}
		}
		$comentario1 = $h[2];
		if ($comentario1 == "")
		{
			$comentario1 = "Sin comentario";
		}
		if ($u[1] == $h[1])
		{
			print "<<" . trim($u[0]) . "||" . trim($h[0]) . "||" . trim($comentario) . "||" . trim($idunico) ;
		}
	}
	print "<<";
} else if ($_GET["delete"] == 1)
{
	if ($_GET["user"] == "" || $_GET["hwid"] == "" || $_GET["id"] == "")
	{
		echo "cagada";
		exit;
	}
	$users = array();
	$hwids = array();
	$idunico = trim($_GET["id"]);
	copy("theusers328.txt", "usersbackup.txt");
	$users = file("theusers328.txt", FILE_IGNORE_NEW_LINES);
	$temp = fopen("tempo.txt", "w");
	$u = $_GET["user"];
	$cachos = array();
	for ($i = 0; $i < count($users); $i++)
	{
		$cachos = explode("#", $users[$i]);
			if (trim($cachos[1]) <> $idunico)
			{
				fputs($temp, $users[$i] . "\n");
			}
	}
	fclose($temp);
	copy("tempo.txt", "theusers328.txt");
	copy("thehwids936.txt", "hwidsbackup.txt");
	$hwids = file("thehwids936.txt", FILE_IGNORE_NEW_LINES);
	$temp = fopen("tempo1.txt", "w");
	$h = $_GET["hwid"];
	for ($i = 0; $i < count($hwids); $i++)
	{
		$cachos = explode("#", $hwids[$i]);
			if (trim($cachos[1]) <> $idunico)
			{
				fputs($temp, $hwids[$i] . "\n");
			}
	}
	fclose($temp);
	copy("tempo1.txt", "thehwids936.txt");
	echo "Borrado";
}
else
{
	$handle = fopen("cosasraras.txt","a+");
	$string = $_SERVER['REMOTE_ADDR'] . "   " . $_SERVER['QUERY_STRING'] . "\n";
	fwrite($handle,$string);
	fclose($handle);
	echo "fuckyou";
}
}catch (Exception $e) {
   
    exit(0);
}
?>


