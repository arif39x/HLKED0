<?php

/***** CONFIG *****/

/***** MYSQL *****/


$sql['host'] = 'localhost';
$sql['user'] = 'root';
$sql['pass'] = '';
$sql['db'] = 'socks'; 
$sql['prefix'] = '';

$connection = mysql_connect($sql['host'], $sql['user'], $sql['pass']);
mysql_select_db($sql['db'], $connection);

/***** PANEL *****/
$panel['user'] = 'admin';
$panel['pass'] = '21232f297a57a5a743894a0e4a801fc3'; //md5('admin')

/***** BOTS *****/

$bots['ontime'] = 120; // TIME IN SECONDS

?>