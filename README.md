# mini-minecraft-pandemic-at-the-disco
mini-minecraft-pandemic-at-the-disco created by GitHub Classroom


Physics

I implemented the camera by linearly interpolating the angle the with the mouse coordinate on the screen. For the x-direction to allow the player to scroll infinitely, I made it so that every time the mouse reaches one edge of the screen it is moved back to the other edge. 

For movement, I just applied the orientation matrix to the velocity vector, and in non-flight mode, instead of cancelling the y-component every time, I just didn't rotate by phi which has the same effect. I did have to tweak the gravity and jump velocity quite a bit to make it feel natural. For jumping to prevent a player from double jumping, they are only allowed to jump if they or on a non-empty block and are very close to it. I also had to use mouse release events to allow for multiple movements at once. 

I had the most trouble with collision. Even now, rarely the player can enter a block, probably because the computer starts chugging and the tick gets longer. I made the player slightly narrower just so that the rays don't hit the edges of a block when you're facing head on. I also make sure not to move the entire distance received from the grid march so as to not go to the edge of the block. If the movement is sufficient small I just stop the player just to ensure the player can't go into a block. There might still be times when it fails especially when we have to load a new chunk as the dT gets very long, but otherwise it mostly works.

Finally, for placing and deleting blocks, I cast a ray from the camera to the block and delete it if left clicked. If the user right clicked, I also get the interface axis and then compare which whether the player is at a higher or lower coordinate in that axis to decide where to place the block. 


Terrain (Alex)

For the mountain biome, I used a combination of perlin noise and fbm to achieve mountain ranges that only popped up sparsely in the landscape. From there, worley noise is also layered onto the mountain noise to give the flatter parts more flavor. 

For the grasslands biome, I used a combination of worley noise and FBM layered on top of each other to give the appearance of irregular hills. 

From there, I combined the noise functions using a scaled up perlin noise function and mixing the two biomes. 

For additional flavor, I overlayed more noise onto the map to determine which blocks should be dirt (adding noise to the grass and mountain biomes) and also used the same noise function to determine the random generation of ice on top of the mountains. 

Texturing (Alex)
I implemented the UV mapping from the world cubes to the texture. Afterwards, I also implemented a separate interleaved VBO specifically for the transparent UV textures. Finally, I added flags to "fluid textures" such as water and lava to move the UVs along with world time. 

Boids (Alex)
In order to get dynamically flocking birds in the world, I used the boid algorithm. To keep things simple, I only used the 2D variant of the boid algorithm in the X-Z axes (the third dimension will be explained shortly). For the third dimension of movement, I sampled the terrain at tick() and made sure that the boids would always adjust their altitude to avoid hitting terrain. Next, I also implemented a random coloring scheme to color the wing tips of the birds. Finally, I implemented a spawn/despawn system so that birds despawn at a distance of around 120 units. Immediately after a flock is despawned, another one is spawned within a 60 unit radius of the player in a randomized direction. 

