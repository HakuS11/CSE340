import static org.junit.jupiter.api.Assertions.*;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

public class chatGPTtc {

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

    //testing age -5
    @Test
    public void TC_1() { //age/weight/height/gender
        System.setIn(new ByteArrayInputStream("-5\n150\n70\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Age must be between 1 and 119."));
    }
    
    //testing age 0
    @Test
    public void TC_2() {
        System.setIn(new ByteArrayInputStream("0\n130\n65\nF\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Age must be between 1 and 119."));
    }
    
    //testing age 121
    @Test
    public void TC_3() {
    	System.setIn(new ByteArrayInputStream("121\n180\n75\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Age must be between 1 and 119."));
    }
    
    //testing weight -10
    @Test
    public void TC_4() {
        System.setIn(new ByteArrayInputStream("30\n-10\n60\nF\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Weight must be between 1 and 999 pounds."));
    }
    
    //testing weight 0
    @Test
    public void TC_5() {
        System.setIn(new ByteArrayInputStream("25\n0\n70\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Weight must be between 1 and 999 pounds."));
    }
    
    //testing weight 1001
    @Test
    public void TC_6() {
        System.setIn(new ByteArrayInputStream("35\n1001\n65\nF\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Weight must be between 1 and 999 pounds."));
    }
    
    //testing height -1
    @Test
    public void TC_7() {
        System.setIn(new ByteArrayInputStream("28\n160\n-1\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Height must be between 1 and 99 inches."));
    }
    
    //testing height 0
    @Test
    public void TC_8() {
        System.setIn(new ByteArrayInputStream("45\n140\n0\nF\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Height must be between 1 and 99 inches."));
    }
    
    //testing height 0
    @Test
    public void TC_09() {
        System.setIn(new ByteArrayInputStream("40\n200\n101\nM\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Height must be between 1 and 99 inches."));
    }
    
    //testing gender X
    @Test
    public void TC_10() {
        System.setIn(new ByteArrayInputStream("25\n150\n65\nX\n".getBytes()));
        Assignment2.main(null);
        assertTrue(outContent.toString().contains("Error: Invalid gender. Please enter 'M' or 'F'."));
    }
    
}
