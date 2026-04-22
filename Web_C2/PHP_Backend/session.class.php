<?php
	/* Copyright © 2012 by Chrystal */
	
	if(!defined('__CP__')) { die(); }
	
	class Session
	{
		private $SessionKey;
		
		private function gID()
		{
			$qInformation  = gethostbyaddr($_SERVER['REMOTE_ADDR']);
			$qInformation .= $_SERVER['REMOTE_ADDR'];
			$qInformation .= $_SERVER['HTTP_USER_AGENT'];
			$qInformation .= ADMIN_PASSWORD;
			
			return md5($qInformation);
		}
		
		public function CheckSession()
		{
			if($this->HasSession() == false)
			{
				if($this->CheckLogin() == true)
				{
					header('Location: ' . $_SERVER['REQUEST_URI']);
					return true;
				} else { return false; }
			} else { return true; }
		}
		
		private function CheckLogin()
		{
			if($_SERVER['REQUEST_METHOD'] == 'POST')
			{
				$Password = $_POST['password'];
				
				if($Password == ADMIN_PASSWORD)
				{
					return $this->CreateSession();
				} else { return false; }
			} else { return false; }
		}
		
		private function CreateSession()
		{
			return setcookie(ST_SESSION_NAME, $this->gID(), (time() + 900), '/', ('.' . $_SERVER['HTTP_HOST']));
		}
		
		public function HasSession()
		{
			$this->SessionKey = $_COOKIE[ST_SESSION_NAME];
			
			if($this->SessionKey == $this->gID())
			{
				return true;
			} else {
				return false;
			}
		}
		
		public function RefreshSession()
		{
			if ($this->HasSession())
				$this->CreateSession();
		}
		
		public function DestroySession()
		{
			$this->SessionKey = $_COOKIE[ST_SESSION_NAME];
			
			if($this->SessionKey)
			{
				setcookie(ST_SESSION_NAME, NULL, (time() - 1), '/', ('.' . $_SERVER['HTTP_HOST']));
				unset($this->SessionKey);
				unset($_COOKIE[ST_SESSION_NAME]);
			}
		}
	}
?>