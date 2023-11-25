/*
**           .d888
**          d88P"
**          888
**  .d88b.  888888 .d8888b
** d88P"88b 888   d88P"
** 888  888 888   888
** Y88b 888 888   Y88b.
**  "Y88888 888    "Y8888P
**      888
** Y8b d88P
**  "Y88P"
**
** Copyright (C) 2023 doublegsoft.open
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#include "gfc.h"

const char* params[] = {
  "--model=../../testdata/jvm_test.modelbase",
  "--license=../../testdata/LICENSE",
  "--output-root=/Users/christian/Downloads/test_jvm",
  "--template-root=/Volumes/EXPORT/opt/modelbase-studio-4.0/data/archetypes/APPBASE+KUI@STDBIZ-2.0/java-appbase+model-2.0",
  "--globals={"
  "\"parentApplication\":\"$PARAPP\","
  "\"application\":\"jvm\","
  "\"namespace\":\"hello.jvm\","
  "\"artifact\":\"hello-jvm\","
  "\"version\":\"1.0.0\","
  "\"description\":\"应用程序描述\","
  "\"naming\":\"com.doublegsoft.jcommons.programming.java.JavaConventions\","
  "\"language\":\"java\","
  "\"imports\":"
  "[],"
  "\"dependencies\":"
  "[]"
  "}",
};

int main(int argc, char* argv[])
{
  gfc_jvm_main("-Djava.class.path=/Volumes/EXPORT/opt/modelbase-studio-4.0/lib/protosys-plugin-modelbase-4.5-shaded.jar",
      "org/doublegsoft/protosys/modelbase/ModelbasePlugin", 5, params);
  return 0;
}
