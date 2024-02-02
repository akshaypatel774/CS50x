SELECT avg(energy) as Average_Energy FROM songs WHERE artist_id = (SELECT id from artists WHERE name = "Drake");
