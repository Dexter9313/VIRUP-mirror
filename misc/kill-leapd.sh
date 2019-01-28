#!/bin/bash

ps aux | grep leap | awk '{print $2}' | sed '$d' | xargs sudo kill -9
