<?php
error_reporting(E_ALL);
include('inc/config.php');
include('inc/functions.php');
include('inc/codes.php');
?>
<!DOCTYPE HTML>
<html>
<head>
<meta charset="utf-8">
<title>RevProxy</title>
<link rel="stylesheet" type="text/css" href="stylesheet.css" />
<link href="http://ajax.googleapis.com/ajax/libs/jqueryui/1.8/themes/base/jquery-ui.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="http://code.jquery.com/jquery-latest.min.js"></script>
<script type="text/javascript" src="http://ajax.googleapis.com/ajax/libs/jqueryui/1.8.16/jquery-ui.min.js"></script>
<script type="text/javascript">
$(function() {
	$("#accordion").accordion({
		collapsible: true,
		animated: false
	}).find("h3:first").click().blur();
	$("#accordion").accordion('option', 'animated', true);
});
</script>
</head>
<body>
<header id="bar">
<?php
	if(isset($_SESSION['pornopanel']))
	{
		echo '<span class="right">logged in as <span class="bold">'.$_SESSION['pornopanel'].'</span> &mdash; '.date('d.m.Y H:i', time()).'</span>';
    }
?>
</header>
<header></header>
<article id="all">
<nav>
	<h2 class="headline"><img src="images/main_icon.png" alt="main" /> <img src="images/mainmenu.png" alt="main menu" /></h2>
    <ul class="navi">
    	<li><a href="index.php">overview</a></li>
        <li><a href="tasks.php">taskmanagement</a></li>
        <li><a href="search.php">search bots</a></li>
        <li><a href="settings.php">settings</a></li>
    </ul>
    <h2 class="headline"><img src="images/stats_icon.png" alt="stats" /> <img src="images/statistics.png" alt="statistics" /></h2>
    <div class="box">
        <ul>
            <li>Online Bots: <span class="green"><?php echo CountOnlineBots(); ?></span></li>
            <li>Offline Bots: <span class="red"><?php echo CountOfflineBots(); ?></span></li>
            <li>Dead Bots: <span class="orange"><?php echo CountDeadBots(); ?></span></li>
            <li class="border"></li>
            <li>All Bots: <span class="bold"><?php echo CountAllBots(); ?></span></li>
        </ul>
    </div>
    <div class="hidden"></div>
</nav>
<article id="content">