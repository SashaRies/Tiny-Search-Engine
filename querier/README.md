# README.md - TSE Querier
## Author: Sasha Ries

## Implementation Notes

The querier successfully:

1. Reads the index file produced by the Indexer
2. Processes queries from stdin, with proper validation
3. Searches the index for documents matching the query
4. Ranks and displays documents in decreasing order by score

## Assumptions

While developing the querier, I made the following assumptions:

1. The pagedirectory and index file correspond to eachother
2. The pageDirectory was created by the TSE Crawler and follows its file format
3. The index file was created by the TSE Indexer and follows its format
4. Memory allocation requests will succeed, though proper error handling is in place
5. The ranking score for documents is based on the sum of word occurrences
6. Operators "and" and "or" are case-insensitive (e.g., "AND", "And", and "and" are equivalent)

## Additional Features

Beyond the basic requirements, I implemented:

1. A prompt for the user only when stdin is from the keyboard

## Known Limitations

While my implementation meets all the requirements in the specification, it has some inherent limitations:

1. It does not handle synonyms or stemming (e.g., "run" and "running" are treated as different words)
2. The ranking algorithm is simple