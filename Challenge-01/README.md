# Challenge 01 - frog

## Initial Analysis

We're given a Python pygame-based frog game. Looking at the source code, the objective is to move the frog to the center to get the flag.

<img width="1616" height="841" alt="Image" src="https://github.com/user-attachments/assets/348a051f-003e-4ee9-8fcd-e9d788c85700" />

## Code Review

Examining the `frog.py` source code, I noticed something interesting in the `Block` class constructor - there's a `passable` parameter that determines if a block can be walked through.

<img width="785" height="400" alt="Image" src="https://github.com/user-attachments/assets/d852e77d-51c0-456c-888e-840fee2afe85" />

Looking at the `BuildBlocks()` function, most blocks are set to `passable=False`, but there are two exceptions:
- Block(15, 4, True) 
- Block(13, 10, True)

## Solution Approach

Instead of solving the maze normally, I modified the `draw()` method in the `Block` class to only render passable blocks:

```python
def draw(self, surface):
    if self.passable:
        surface.blit(self.image, self.rect)
```

<img width="1112" height="664" alt="Image" src="https://github.com/user-attachments/assets/722958d0-0af3-40ee-a3d1-96de37376e36" />

## Execution

Running the modified code with `python frog.py` reveals only the passable blocks, showing the hidden path through the maze.

<img width="1004" height="784" alt="Image" src="https://github.com/user-attachments/assets/119a9d1f-093d-4f18-a639-cf379bab6a19" />

Moving the frog through the passable blocks to reach the victory tile at triggers the flag generation.

<img width="997" height="793" alt="Image" src="https://github.com/user-attachments/assets/89baf319-1c43-43e0-bea4-f6e76429dc05" />

## Flag

The `GenerateFlagText()` function XORs the encoded string with a key based on the victory position coordinates, revealing the flag.

**Flag:** `welcome_to_11@flare-on.com`