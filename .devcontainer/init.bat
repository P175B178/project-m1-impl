@echo off
if not exist .env (
    copy .env.sample .env >nul
    echo Created .env from .env.sample - fill in your Pi connection details.
)
