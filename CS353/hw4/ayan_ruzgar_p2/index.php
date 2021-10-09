<!DOCTYPE html>
<html>

<body>

<script type="text/javascript">
  function checkEmpty() {
    var a = document.forms["LoginForm"]["username"].value;
    var b = document.forms["LoginForm"]["password"].value;
    if (a == null || a == "" || b == null || b == "") {
      alert("Username or password field is empty. Please fill them before submitting.");
      return false;
    }
	return true;
  }
</script>

<form action="login.php" name="LoginForm" method="post" onsubmit="return checkEmpty()" style='margin: auto; text-align: center'>
	
	<div>
		<label>Username:</label>
		<input type="text" name="username" placeholder="username">
	</div>
	
	<br/>
	
	<div>
		<label>Password:</label>
		<input type="password" name="password" placeholder="password">
	</div>
	
	<br/>
	
	<div>
		<button type="submit" name="logIn">Login</button>
	</div>
</form>
</body>
</html>