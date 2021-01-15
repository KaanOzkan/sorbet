#!/bin/bash
exec 2>&1
D=test/cli/rbi-flakiness
for run in {1..10}; do
    main/sorbet --silence-dev-message $D/*.rbi $D/test.rb
done