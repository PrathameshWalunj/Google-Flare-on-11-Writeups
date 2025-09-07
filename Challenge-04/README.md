# Challenge 04 - Meme Maker 3000

## Initial Analysis

We're given an HTML file containing a meme generator with heavily obfuscated JavaScript. The page lets you generate memes with canned messages about FLARE-ON and reverse engineering in general(some of them are good).

<img width="1919" height="1019" alt="Image" src="https://github.com/user-attachments/assets/3d8c5c94-e229-4b86-a9bd-694799b9a87a" />

## Diving into the Obfuscated Mess

Opening the source reveals some seriously mangled JavaScript. Variable names like `a0f`, `a0g`, `a0k` and hex-encoded strings everywhere - these are classic obfuscation tactics that make JS ugly(sorry).

<img width="1919" height="1015" alt="Image" src="https://github.com/user-attachments/assets/1c4a780a-ea94-4871-aa07-a6cda5a3ce12" />

Looking at this mess, I knew I needed to find the important parts. In web challenges, the money shot is usually in functions that output data to the user.

## The Hunt for Output

My strategy was to search for `alert` calls. If there's a flag to be found, it's probably getting shown via `alert()` or similar.

I Found the `a0k()` function that contains an `alert()` call. Time to see what this bad boy actually does.

<img width="1919" height="1004" alt="Image" src="https://github.com/user-attachments/assets/45f74d0c-fe3a-4bac-be47-4728af877083" />

## Dynamic Analysis FTW

Instead of manually deobfuscating everything, I used the browser's developer console to peek under the hood:

<img width="1919" height="978" alt="Image" src="https://github.com/user-attachments/assets/fceb11f5-0e3b-497e-bf9b-eed7fcaf5ac7" />

```javascript
console.log(a0k.toString());
```

This reveals the actual logic of `a0k()` - it checks specific conditions on the meme captions and builds a flag string if they're met.

## Reconstructing the Flag

I just reimplemented the flag construction logic directly in the console:

<img width="1915" height="1018" alt="Image" src="https://github.com/user-attachments/assets/e5b4ec66-5543-4a13-89d7-cf93329433c4" />

The key insight was that `a0k()` grabs specific characters from various arrays (`a0c`, `a0e`) and text content, then concatenates them into the flag string.

## The Solution

```javascript
(() => {
  
  const b = a0c[0x0e];
  const c = a0c[a0c.length - 1];
  const d = a0c[0x16];
  const a = Object.keys(a0e)[5];

  // Build f exactly like in a0k()
  let f = d[0x3] + 'h' + a[0xa] + b[0x2] + a[0x3] + c[0x5] +
          c[c.length - 1] + '5' + a[0x3] + '4' + a[0x3] +
          c[0x2] + c[0x4] + c[0x3] + '3' + d[0x2] + a[0x3] +
          'j4' + a0c[0x1][0x2] + d[0x4] + '5' + c[0x2] + d[0x5] +
          '1' + c[0xb] + '7' + a0c[0x15][0x1] +
          b.replace(/\x20/g, '-') + a[0xb] +
          a0c[0x4].slice(0xc, 0xf);

  f = f.toLowerCase();

  console.log(
    atob(a0p(0x14e2b) + a0p(0x4c22) + 'YXRpb2' +
         a0p(0x1708e) + a0p(0xaa98) + a0p(0x16697) +
         a0p(0x109c4)) + f
  );
})();
```

**Flag:** `wh0a_it5_4_cru3l_j4va5cr1p7@flare-on.com`

