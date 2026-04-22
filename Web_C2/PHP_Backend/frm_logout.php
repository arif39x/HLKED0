<?php
	/* Copyright © 2012 by Chrystal */
	
	if(!defined('__CP__')) { die(); }
	
	class Template_Logout
	{
		private $Content = NULL;
		private $Title = 'Logout';
		
		public function __construct()
		{
			$this->Content .= ('<center><b>You have been successfully logged out !</b></center>');
			$this->Content .= ('<script language="javascript">
									function refresh() { location.href="index.php"; }
									setTimeout(\'refresh()\', 2000);
								</script>');
		}
		
		public function GetContent()
		{
			global $Session;
			$Session->DestroySession();
			
			return $this->Content;
		}
		
		public function GetTitle()
		{
			return $this->Title;
		}
	}
?>