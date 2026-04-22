<?php
	/* Copyright © 2012 by Chrystal */
	
	require_once ('../system/global.php');
	include_once ('../system/session.class.php');
	
	$Session = new Session();
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<title>Zemra Live (Administration)</title>
		<meta http-equiv='Content-Type' content='text/html; charset=UTF-8' />
		<link rel='stylesheet' href='resources/css/style.css' type='text/css' />
		<link rel='shortcut icon' href='resources/images/favicon.ico' type='image/x-icon' />
		
		<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jquery/1.6.1/jquery.min.js"></script>
		<script type="text/javascript" src="resources/script/highcharts.js"></script>
		
		<script type="text/javascript">
			var _RefreshSession = function () { $.get("index.php?q=refresh_session"); };
			window.setInterval(_RefreshSession, (2 * 60 * 1000));
		</script>
	</head>
	
	<?php
		if($Session->CheckSession())
		{
			include_once ('templates/frm_main.php');
		} else {
			include_once ('templates/frm_login.php');
		}
	?>
</html>