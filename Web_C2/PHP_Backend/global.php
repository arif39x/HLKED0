<?php
	/* Copyright © 2012 by Chrystal */
	
	header('content-type: text/html; charset=utf-8');
	
	error_reporting(FALSE);
	ini_set('display_errors', FALSE);
	set_time_limit(30);
	ob_start();
	
	DEFINE ('__CP__', TRUE);
	DEFINE ('__VERSION__', '1.0.0.0');
	
	DEFINE ('ST_SESSION_NAME', 'ZEMRA');
	DEFINE ('ST_SOCK_CTIME', 2);
	DEFINE ('ST_SHOW_COUNT', 40);
	DEFINE ('ST_BOT_TIME_ON', 100);
	
	DEFINE ('MESSAGE_INFO_FAILED', FALSE);
	DEFINE ('MESSAGE_INFO_SUCCESS', TRUE);
	
	include_once ('config.include.php');
	include_once ('geoip/geoip.inc');
	include_once ('base.class.php');
	include_once ('database.class.php');
	
	$Base = new Base();
	$DB = new Database();
?>