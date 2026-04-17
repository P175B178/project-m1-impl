#!/bin/sh
if [ ! -f .env ]; then
  cp .env.sample .env
  echo "Created .env from .env.sample — fill in your Pi connection details."
fi
