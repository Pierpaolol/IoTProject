<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
<?php
$conn = new mysqli("localhost", "root", "", "sensordata");
?>
<title>Qualità dell'Aria</title>
<link rel="stylesheet" href="style.css">
</head>
<body>
    <?php
    $sql = mysqli_query($conn, "SELECT * FROM sensordata");
    $number_of_rows = mysqli_num_rows( $sql);

    //echo strval($number_of_rows);
    echo "<table border=1>";
    echo    "<caption align=top><em>Tabella</em></caption>";
    echo    "<tr>";
    echo    "<th rowspan=1>Data ed ora</th>";
    echo    "<th rowspan=1>Temperatura</th>";
    echo    "<th rowspan=1>Umidità</th>";
    echo    "<th rowspan=1>Gas</th>";
    echo    "<th rowspan=1>Allerta</th>";
    echo    "<tr>";
    for ($i = $number_of_rows; $i >= 50;$i--){
        $query = mysqli_query($conn,"SELECT * FROM sensordata WHERE id = '$i'");
        $row = mysqli_fetch_array($query);
        echo    "<tr>";
        echo    "<th rowspan=1>".$row['reading_time']."</th>";
        echo    "<th rowspan=1>".$row['temperature']."</th>";
        echo    "<th rowspan=1>".$row['humidity']."</th>";
        echo    "<th rowspan=1>".$row['gasVal']."</th>";
        echo    "<th rowspan=1>".$row['alarm']."</th>";
        echo    "</tr>";
    }
    echo "</table>";
    ?>
</body>
</html>