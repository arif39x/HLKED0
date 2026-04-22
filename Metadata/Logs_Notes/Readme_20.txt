These are the files that have to go on the web server.
File Monster.php is what does all the work, validates the use to registered users
add and delete users, etc..
Version.php file is where version indicates, when you do an update there
to increase the version number.
Md5.php file is used by the program to generate a md5 Extractor only one I identify
each user to delete from the list safely.
The part of code Monster.php where the second verification is the time to create the stub
is where you have to add a key for each new version, so only registered users properly
in the list may generate the stub. eg

VERSION 3.0

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
}

Version 3.0 and 3.1

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
		if ($_version == 3.1)
		{
			echo ansiencript("passwordfordrelease3.1");
		}
	} else {
		echo "cagada";
		exit;
	}
}