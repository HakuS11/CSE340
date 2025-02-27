import static org.junit.jupiter.api.Assertions.*;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

public class GPTtestCases {

    private final ByteArrayOutputStream outContent = new ByteArrayOutputStream();
    private final PrintStream originalOut = System.out;

    @BeforeEach
    public void setUpStreams() {
        System.setOut(new PrintStream(outContent));
    }

    @org.junit.jupiter.api.AfterEach
    public void restoreStreams() {
        System.setOut(originalOut);
    }

    //testing age -1
    @Test
    public void TC_1() {
        System.setIn(new ByteArrayInputStream("-1\n65\n150\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Age must be between 1 and 119."));
    }

    //testing age  0
    @Test
    public void TC_2() {
        System.setIn(new ByteArrayInputStream("0\n65\n150\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Age must be between 1 and 119."));
    }

    //testing age 1
    @Test
    public void TC_3() {
        System.setIn(new ByteArrayInputStream("1\n65\n150\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Your BMR is:"));
    }

    //testing age 2
    @Test
    public void TC_4() {
        System.setIn(new ByteArrayInputStream("2\n65\n150\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Your BMR is:"));
    }

    //testing age 118
    @Test
    public void TC_5() {
        System.setIn(new ByteArrayInputStream("118\n65\n150\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Your BMR is:"));
    }

    //testing age 119
    @Test
    public void TC_6() {
        System.setIn(new ByteArrayInputStream("119\n65\n150\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Your BMR is:"));
    }

    //testing age 120
    @Test
    public void TC_7() {
        System.setIn(new ByteArrayInputStream("120\n65\n150\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Age must be between 1 and 119."));
    }

    //testing height -1
    @Test
    public void TC_8() {
        System.setIn(new ByteArrayInputStream("25\n-1\n150\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Height must be between 1 and 99 inches."));
    }

    //testing height 0
    @Test
    public void TC_9() {
        System.setIn(new ByteArrayInputStream("25\n0\n150\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Height must be between 1 and 99 inches."));
    }

    //testing height 1
    @Test
    public void TC_10() {
        System.setIn(new ByteArrayInputStream("25\n1\n150\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Your BMR is:"));
    }

    //testing height 2
    @Test
    public void TC_11() {
        System.setIn(new ByteArrayInputStream("25\n2\n150\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Your BMR is:"));
    }

    //testing height 98
    @Test
    public void TC_12() {
        System.setIn(new ByteArrayInputStream("25\n98\n150\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Your BMR is:"));
    }

    //testing height 99
    @Test
    public void TC_13() {
        System.setIn(new ByteArrayInputStream("25\n99\n150\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Your BMR is:"));
    }

    //testing height 100
    @Test
    public void TC_14() {
        System.setIn(new ByteArrayInputStream("25\n100\n150\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Height must be between 1 and 99 inches."));
    }

    //testing weight -1
    @Test
    public void TC_15() {
        System.setIn(new ByteArrayInputStream("25\n65\n-1\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Weight must be between 1 and 999 pounds."));
    }

    //testing weight 0
    @Test
    public void TC_16() {
        System.setIn(new ByteArrayInputStream("25\n65\n0\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Weight must be between 1 and 999 pounds."));
    }

    //testing weight 1
    @Test
    public void TC_17() {
        System.setIn(new ByteArrayInputStream("25\n65\n1\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Your BMR is:"));
    }

    //testing weight 2
    @Test
    public void TC_18() {
        System.setIn(new ByteArrayInputStream("25\n65\n2\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Your BMR is:"));
    }

    //testing weight 998
    @Test
    public void TC_19() {
        System.setIn(new ByteArrayInputStream("25\n65\n998\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Your BMR is:"));
    }

    //testing weight 999
    @Test
    public void TC_20() {
        System.setIn(new ByteArrayInputStream("25\n65\n999\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Your BMR is:"));
    }

    //testing weight 1000
    @Test
    public void TC_21() {
        System.setIn(new ByteArrayInputStream("25\n65\n1000\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Weight must be between 1 and 999 pounds."));
    }

    //testing gender X
    @Test
    public void TC_22() {
        System.setIn(new ByteArrayInputStream("25\n65\n150\nX\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Invalid gender. Please enter 'M' or 'F'."));
    }

    //testing gender blank
    @Test
    public void TC_23() {
        System.setIn(new ByteArrayInputStream("25\n65\n150\n\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Invalid gender. Please enter 'M' or 'F'."));
    }

    //testing gender M
    @Test
    public void TC_24() {
        System.setIn(new ByteArrayInputStream("25\n65\n150\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Your BMR is:"));
    }

    //testing gender F
    @Test
    public void TC_25() {
        System.setIn(new ByteArrayInputStream("25\n65\n150\nF\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Your BMR is:"));
    }
}
