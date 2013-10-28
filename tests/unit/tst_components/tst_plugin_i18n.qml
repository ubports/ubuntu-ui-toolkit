/*
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
     name: "i18nAPI"

     function test_0_tr() {
         var testValue = "Hello World!";
         var readValue = i18n.tr(testValue);
         compare(testValue,readValue,"can use tr");
     }

     function test_1_bindtextdomain() {
         var testValue = "../../../examples/ubuntu-ui-toolkit-gallery/po/";
         signalSpy.signalName = "domainChanged";
         i18n.bindtextdomain("gallery",testValue);
         signalSpy.wait()
     }

     function test_0_domain() {
         compare(i18n.domain,"","domain is empty by default");
         var testValue = "gallery";
         signalSpy.signalName = "domainChanged";
         i18n.domain = testValue;
         signalSpy.wait()
         compare(i18n.domain,testValue,"can set/get domain");
     }

     function test_0_language() {
         var testValue = "fi";
         signalSpy.signalName = "languageChanged";
         i18n.language = testValue;
         signalSpy.wait()
         compare(i18n.language,testValue,"can set/get language");
     }

     /*
     function test_0_dtr() {
         expectFail("","a directory structure change is required in the project tree")
//TODO: add call to bindtextdomain
         var testENValue = "Call";
         i18n.language = "nl";
         var readValue = i18n.dtr("gallery",testENValue);
         var testFIValue = "Bellen";
         compare(readValue,testFIValue,"can use dtr");
     }
     */

     SignalSpy {
         id: signalSpy
         target: i18n
     }

}
