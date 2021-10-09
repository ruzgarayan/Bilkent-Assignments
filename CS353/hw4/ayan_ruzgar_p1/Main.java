import java.sql.*;

public class Main {

    static Connection connection;
    static Statement statement;

    public static void insertStudent(String sid, String sname, String bdate, String address,
                                     String scity, String year, String gpa, String nationality)
    {
        try {
            statement.executeUpdate("insert into student VALUES('" +
                    sid + "', '" + sname + "', DATE '" + bdate + "', '" + address + "', '" +
                    scity + "', '" + year + "', '" + gpa + "', '" + nationality +"');");
        } catch (SQLException throwables) {
            System.out.println("Error when inserting the student with sid= " + sid + " to the database. Exiting. " +
                    "Error message: + " + throwables.getMessage());
            System.exit(1);
        }
    }

    public static void insertCompany(String cid, String cname, String quota)
    {
        try {
            statement.executeUpdate("insert into company VALUES('" +
                    cid + "', '" + cname + "', '" + quota + "');");
        } catch (SQLException throwables) {
            System.out.println("Error when inserting the company with cid= " + cid + " to the database. Exiting. " +
                    "Error message: + " + throwables.getMessage());
            System.exit(1);
        }
    }

    public static void insertApply(String sid, String cid)
    {
        try {
            statement.executeUpdate("insert into apply VALUES('" +
                    sid + "', '" + cid + "');");
        } catch (SQLException throwables) {
            System.out.println("Error when inserting the application with sid= " + sid + " and cid= "
                    + cid + " to the database. Exiting. \nError message: " + throwables.getMessage());
            System.exit(1);
        }
    }

    public static void deleteExistingTables()
    {
        try {
            statement.executeUpdate("DROP TABLE if exists apply");
        } catch (SQLException throwables) {
            System.out.println("Error while dropping the apply table, exiting. \nError message: + " + throwables.getMessage());
            System.exit(1);
        }

        try {
            statement.executeUpdate("DROP TABLE if exists student");
        } catch (SQLException throwables) {
            throwables.printStackTrace();
            System.out.println("Error while dropping the student table, exiting. \nError message: + " + throwables.getMessage());
            System.exit(1);
        }

        try {
            statement.executeUpdate("DROP TABLE if exists company");
        } catch (SQLException throwables) {
            System.out.println("Error while dropping the company table, exiting. \nError message: + " + throwables.getMessage());
            System.exit(1);
        }

        System.out.println("Existing tables have been deleted successfuly.");
    }

    public static void createTables()
    {
        try {
            statement.executeUpdate("CREATE TABLE if not exists student(" +
                    "sid CHAR(12), " +
                    "sname VARCHAR(50)," +
                    "bdate DATE," +
                    "address VARCHAR(50)," +
                    "scity VARCHAR(20)," +
                    "year CHAR(20)," +
                    "gpa FLOAT(24)," +
                    "nationality VARCHAR(20)," +
                    "PRIMARY KEY(sid)" +
                    ") ENGINE=InnoDB;");
        } catch (SQLException throwables) {
            System.out.println("Error while creating the student table, exiting. \nError message: + " + throwables.getMessage());
            System.exit(1);
        }

        try {
            statement.executeUpdate("CREATE TABLE if not exists company(" +
                    "cid CHAR(8), " +
                    "cname VARCHAR(20)," +
                    "quota INT(5)," +
                    "PRIMARY KEY(cid)" +
                    ") ENGINE=InnoDB;");
        } catch (SQLException throwables) {
            System.out.println("Error while creating the company table, exiting. \nError message: + " + throwables.getMessage());
            System.exit(1);
        }

        try {
            statement.executeUpdate("CREATE TABLE if not exists apply(" +
                    "sid CHAR(12), " +
                    "cid CHAR(8)," +
                    "PRIMARY KEY(cid, sid)," +
                    "FOREIGN KEY (sid) REFERENCES student(sid) ON DELETE CASCADE," +
                    "FOREIGN KEY (cid) REFERENCES company(cid) ON DELETE CASCADE" +
                    ") ENGINE=InnoDB;");
        } catch (SQLException throwables) {
            System.out.println("Error while creating the apply table, exiting. \nError message: + " + throwables.getMessage());
            System.exit(1);
        }

        System.out.println("New tables have been created successfuly.");
    }

    public static void main(String[] args) {
        try {
            Class.forName("com.mysql.cj.jdbc.Driver");
        } catch (ClassNotFoundException ex) {
            System.out.println("Cannot find the jdbc driver, exiting");
            System.exit(1);
        }

        try {
            connection = DriverManager.getConnection("mydatabase",
                    "myusername", "mypassword");
            System.out.println("Connected to the database successfuly.");
        } catch (SQLException throwables) {
            System.out.println("Cannot connect to the database. \nError message: + " + throwables.getMessage());
            System.exit(1);
        }

        try {
            statement = connection.createStatement();
            System.out.println("Created the statement successfuly.");
        } catch (SQLException throwables) {
            System.out.println("Cannot create a statement. \nError message: + " + throwables.getMessage());
            System.exit(1);
        }

        deleteExistingTables();
        createTables();

        insertStudent("21000001","John", "1999-5-14", "Windy", "Chicago", "senior", "2.33", "US");
        insertStudent("21000002", "Ali", "2001-9-30", "Nisantasi", "Istanbul", "junior", "3.26", "TC");
        insertStudent("21000003", "Veli", "2003-2-25", "Nisantasi", "Istanbul", "freshman", "2.41", "TC");
        insertStudent("21000004", "Ayse", "2003-1-15", "Tunali", "Ankara", "freshman", "2.55", "TC");

        insertCompany("C101", "microsoft", "2");
        insertCompany("C102", "merkez bankasi", "5");
        insertCompany("C103", "tai", "3");
        insertCompany("C104", "tubitak", "5");
        insertCompany("C105", "aselsan", "3");
        insertCompany("C106", "havelsan", "4");
        insertCompany("C107", "milsoft", "2");

        insertApply("21000001", "C101");
        insertApply("21000001", "C102");
        insertApply("21000001", "C103");
        insertApply("21000002", "C101");
        insertApply("21000002", "C105");
        insertApply("21000003", "C104");
        insertApply("21000003", "C105");
        insertApply("21000004", "C107");

        System.out.println("Table population is completed.");
        System.out.println("Now, printing all student information.\n######################################");

        ResultSet queryResults;
        try {
            queryResults = statement.executeQuery("SELECT * FROM student");

            while (queryResults.next())
            {
                System.out.printf("Student{sid=%s, sname=%s, bdate=%s, address=%s, scity=%s, year=%s, gpa=%s, nationality=%s}\n",
                        queryResults.getString("sid"),
                        queryResults.getString("sname"),
                        queryResults.getString("bdate"),
                        queryResults.getString("address"),
                        queryResults.getString("scity"),
                        queryResults.getString("year"),
                        queryResults.getString("gpa"),
                        queryResults.getString("nationality")
                        );
            }
        } catch (SQLException throwables) {
            System.out.println("Cannot query the student table. \nError message: + " + throwables.getMessage());
            System.exit(1);
        }

        System.out.println("######################################\n");

        try {
            connection.close();
            statement.close();
            System.out.println("Closed the connection and the statement successfully.");
        } catch (SQLException throwables) {
            System.out.println("Cannot close the connection and/or the statement. \nError message: + " + throwables.getMessage());
        }
    }
}
