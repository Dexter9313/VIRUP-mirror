::# push artifacts to Gitlab

cd %APPVEYOR_BUILD_FOLDER%
set GITLAB_TAG=%APPVEYOR_REPO_TAG_NAME%
set PACKAGES_URL=https://gitlab.com/api/v4/projects/%GITLAB_PROJECT_ID%/packages/generic/releases
set RELEASES_URL=https://gitlab.com/api/v4/projects/%GITLAB_PROJECT_ID%/releases/%GITLAB_TAG%/assets/links
set EXE_FILE=%RELEASE_DIR_NAME%_setup.exe
set EXE_PATH=innosetup\Output\%EXE_FILE%
set EXE_PACK_URL=%PACKAGES_URL%/%GITLAB_TAG%/%EXE_FILE%
set ZIP_FILE=%RELEASE_DIR_NAME%.zip
set ZIP_PATH=build\Release\%ZIP_FILE%
set ZIP_PACK_URL=%PACKAGES_URL%/%GITLAB_TAG%/%ZIP_FILE%
set HEADER=PRIVATE-TOKEN:^ %GITLAB_PRIVATE_TOKEN%
curl -u appveyor:%GITLAB_DEPLOY_TOKEN_PASS% --upload-file %EXE_PATH% "%EXE_PACK_URL%"
curl -u appveyor:%GITLAB_DEPLOY_TOKEN_PASS% --upload-file %ZIP_PATH% "%ZIP_PACK_URL%"
curl --request POST --header "%HEADER%" --data name="%EXE_FILE%" --data url="%EXE_PACK_URL%" --data link_type="package" "%RELEASES_URL%"
curl --request POST --header "%HEADER%" --data name="%ZIP_FILE%" --data url="%ZIP_PACK_URL%" --data link_type="package" "%RELEASES_URL%"
echo If any error occured, please read https://gitlab.com/Dexter9313/hydrogenvr/-/wikis/tutorials/1-general-topics/9-continuous-integration for more details.
IF NOT DEFINED GITLAB_PROJECT_ID (echo GITLAB_PROJECT_ID is not defined.)
IF NOT DEFINED GITLAB_PRIVATE_TOKEN (echo GITLAB_PRIVATE_TOKEN is not defined.)
IF NOT DEFINED GITLAB_DEPLOY_TOKEN_PASS (echo GITLAB_DEPLOY_TOKEN_PASS is not defined.)

