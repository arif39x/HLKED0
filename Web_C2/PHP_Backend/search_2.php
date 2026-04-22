<?php
include('inc/session.php');
if(!isset($_SESSION['pornopanel']))
{
	header("Location: login.php");
}

include('header.php');




include('footer.php');
?>