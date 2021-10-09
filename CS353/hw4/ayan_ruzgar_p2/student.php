<!DOCTYPE html>
<html>

<head>
    <title>Student Welcome Page</title>
    <style>
      table,
      th,
      td {
        padding: 10px;
        border: 1px solid black;
        border-collapse: collapse;
		margin: auto;
		text-align: center;
      }
    </style>
</head>

<body>
	<div>
		<h1> Applied Companies </h1>
		<?php
			include 'connection.php';
			session_start();
			
			if(empty($_SESSION))
			{
				header("refresh:0;url=index.php");
			}
			
			$sqlQuery = "SELECT apply.cid, company.cname, company.quota FROM apply, company WHERE apply.sid ='" . $_SESSION['sid'] . "' AND apply.cid = company.cid;";
			$queryResult = mysqli_query($connection, $sqlQuery);
			$numRows = mysqli_num_rows($queryResult);
			if ($numRows > 0)
			{
				
				echo "
				<table>
					<thead>
						<tr>
							<th>cid</th>
							<th>cname</th>
							<th>quota</th>
							<th>action</th>
						</tr>
					</thead>
				";
				
				for ($i = 0; $i < mysqli_num_rows($queryResult); $i++)
				{
					$currTuple = mysqli_fetch_assoc($queryResult);
					$cid = $currTuple["cid"];
					echo "
							<tbody>
								<td>" . $cid . "</td>
								<td>" . $currTuple["cname"] . "</td>
								<td>" . $currTuple["quota"] . "</td>
								<td> 
									<form method=\"post\" action=\"cancelApplication.php\">
										<input type=\"hidden\" name=\"cid\" value=\"" . $cid . "\" />
										<button type=\"submit\"> 
											Cancel Application
										</button>
									</form> 
								</td>
							</tbody>
					";
				}
				
				echo "</table>";
			}
			else
			{
				echo "<div> You have not applied to any company yet </div>";
			}
		?>
	</div>
	</br>
	<div>
		Apply for new internship <a href="newApplication.php">here</a>.
	</div>
	</br>
	<div>
		<a href="logout.php">Logout</a>.
	</div>
</body>

</html>