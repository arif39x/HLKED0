<?php
include("./inc/config.php");
include("./inc/funcs.php");
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    if (isset($_POST["loginUsername"]) && (isset($_POST["loginPassword"])))
    {
        session_start();
        $username = $_POST['loginUsername'];
        $passwort = $_POST['loginPassword'];
        $hostname = $_SERVER['HTTP_HOST'];
        $path = dirname($_SERVER['PHP_SELF']);
        if ($username == $admin_user && $passwort == $admin_pass) {
            $_SESSION['angemeldet'] = true;
            echo "{success: true}";
            die("");
       }
    }
    echo "{success: false, errors: { reason: 'Login failed. Try again.' }}";
}
die("");
?>