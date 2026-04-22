<?php
	/* Copyright © 2012 by Chrystal */
	
	if (!defined('__CP__')) { die(); }
	
	class Database
	{
		private $CID;
		
		public function __construct()
		{
			$this->Connect();
		}
		
		public function __destruct()
		{
			$this->Disconnect();
		}
		
		private function Connect()
		{
			$this->CID = @mysql_connect(MYSQL_SERVER, MYSQL_USERNAME, MYSQL_PASSWORD, true);
			
			if(!$this->CID || !@mysql_select_db(MYSQL_DATABASE, $this->CID))
			{
				$nEMessage = @mysql_error();
				if(!$nEMessage) { $nEMessage = 'MySQL Connection Error'; }
				die($nEMessage);
			}
			
			$this->SetEncoding(MYSQL_ENCODING);
		}
		
		private function Disconnect()
		{
			if($this->CID) { @mysql_close($this->CID); }
		}
		
		private function SetEncoding($Charset)
		{
			@mysql_set_charset($Charset, $this->CID);
		}
		
		public function Query($SQL)
		{
			return @mysql_query($SQL);
		}
		
		public function GetIID()
		{
			return @mysql_insert_id();
		}
		
		public function GetRow($qID)
		{
			return @mysql_fetch_assoc($qID);
		}
		
		public function NumRows($qID)
		{
			return @mysql_num_rows($qID);
		}
		
		public function GetRowArray($qID)
		{
			return @mysql_fetch_array($qID);
		}
		
		public function SaveRead($String)
		{
			return @mysql_real_escape_string($String);
		}
	}
?>