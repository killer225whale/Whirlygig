// Whirlygig 3D Volumetric Display

import java.lang.Math.*;

public class Whirlygig {
	private final static int RADIUS = 18; // Includes origin as 0th LED
	private final static int LAYER_SIZE = 25; // Layer dimensions (square)
	private final static int ARRAY_OFFSET = 15; // Offset required for calc
	private final static int SLICES = 120; // Angles required

	/**
	 * Transcribes the LED sequence for a single layer.
	 *
	 * @param	layer	A 2D pixel/voxel int array representing the layer.
	 * @param			A 2D int array with instructions for each slice/LED.
	 */
	public static int[][] transcribeLayer(int[][] layer) {
		int[][] sequence = new int[SLICES][RADIUS];

		// For each slice
		for (int a = 0; a < SLICES; a++) {
			double aDeg = a * 360.0 / SLICES; // get angle in degrees
			double aRad = Math.toRadians(aDeg); // convert angle to radians

			// For each LED in slice
			for (int r = 0; r < RADIUS; r++) {
				// Calculate Cartesian coordinate for angle/radius
				Double x1 = r * Math.cos(aRad);
				Double y1 = r * Math.sin(aRad);
				int x = x1.intValue();
				int y = y1.intValue();

				// Get layer grid values
				int row = ARRAY_OFFSET - y;
				int col = ARRAY_OFFSET - x;

				if (row < 0 || row >= LAYER_SIZE ||
					col < 0 || col >= LAYER_SIZE) {
					sequence[a][r] = 0; // Mask LEDs outside layer grid
				} else {
					sequence[a][r] = layer[row][col];
				}
			}
		}

		return sequence;
	}

	public static void main(String[] args) {
		// Generate a square outline for the layer size
		int[][] layer = new int[LAYER_SIZE][LAYER_SIZE];
		for (int row = 0; row < LAYER_SIZE; row++) {
			for (int col = 0; col < LAYER_SIZE; col++) {
				if (row == 0 || row == LAYER_SIZE-1
					|| col == 0 || col == LAYER_SIZE-1) {
					layer[row][col] = 1;
				} else {
					layer[row][col] = 0;
				}
			}
		}

		// Transcribe and output a single layer
		int[][] sequence = transcribeLayer(layer);
		for (int a = 0; a < SLICES; a++) {
			for (int r = 0; r < RADIUS; r++) {
				System.out.print(sequence[a][r]);
			}
			System.out.print("\n");
		}

		return ;
	}
}
