// Copyright 2017 Yahoo Holdings. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.
package com.yahoo.container.logging;

import com.yahoo.yolean.trace.TraceNode;
import org.junit.Test;

import java.io.IOException;
import java.net.URI;

import static org.junit.Assert.assertEquals;


/**
 * @author frodelu
 */
public class JSONLogTestCase {

    private static String ipAddress = "152.200.54.243";
    private static final String EMPTY_REFERRER = "";
    private static final String EMPTY_USERAGENT = "";

    private AccessLogEntry newAccessLogEntry(final String query) {
        return newAccessLogEntry(query, new Coverage(100,100,100,0));
    }
    private AccessLogEntry newAccessLogEntry(final String query, Coverage coverage) {
        final AccessLogEntry entry = new AccessLogEntry();
        entry.setRawQuery("query="+query);
        entry.setRawPath("");
        entry.setIpV4Address(ipAddress);
        entry.setHttpMethod("GET");
        entry.setHttpVersion("HTTP/1.1");
        entry.setUserAgent("Mozilla/4.05 [en] (Win95; I)");
        entry.setHitCounts(new HitCounts(0, 10, 1234, 0, 10, coverage));
        entry.setHostString("localhost");
        entry.setStatusCode(200);
        entry.setTimeStamp(920880005023L);
        entry.setDurationBetweenRequestResponse(122);
        entry.setReturnedContentSize(9875);

        return entry;
    }

    private static URI newQueryUri(final String query) {
        return URI.create("http://localhost?query=" + query);
    }

    @Test
    public void test_json_log_entry() throws Exception {
        AccessLogEntry entry = newAccessLogEntry("test");

         String expectedOutput =
            "{\"ip\":\"152.200.54.243\"," +
            "\"time\":920880005.023," +
            "\"duration\":0.122," +
            "\"responsesize\":9875," +
            "\"code\":200," +
            "\"method\":\"GET\"," +
            "\"uri\":\"?query=test\"," +
            "\"version\":\"HTTP/1.1\"," +
            "\"agent\":\"Mozilla/4.05 [en] (Win95; I)\"," +
            "\"host\":\"localhost\"," +
            "\"scheme\":null," +
            "\"localport\":0," +
            "\"search\":{" +
            "\"totalhits\":1234," +
            "\"hits\":0," +
            "\"coverage\":{\"coverage\":100,\"documents\":100}" +
            "}" +
            "}";

        assertEquals(expectedOutput, new JSONFormatter(entry).format());
    }

    @Test
    public void test_json_of_trace() throws IOException {
        TraceNode root = new TraceNode("root", 7);
        AccessLogEntry entry = newAccessLogEntry("test");
        entry.setTrace(root);

        String expectedOutput =
                "{\"ip\":\"152.200.54.243\"," +
                "\"time\":920880005.023," +
                "\"duration\":0.122," +
                "\"responsesize\":9875," +
                "\"code\":200," +
                "\"method\":\"GET\"," +
                "\"uri\":\"?query=test\"," +
                "\"version\":\"HTTP/1.1\"," +
                "\"agent\":\"Mozilla/4.05 [en] (Win95; I)\"," +
                "\"host\":\"localhost\"," +
                "\"scheme\":null," +
                "\"localport\":0," +
                "\"trace\":{\"timestamp\":0,\"message\":\"root\"}," +
                "\"search\":{" +
                "\"totalhits\":1234," +
                "\"hits\":0," +
                "\"coverage\":{\"coverage\":100,\"documents\":100}" +
                "}" +
                "}";

        assertEquals(expectedOutput, new JSONFormatter(entry).format());
    }

    @Test
    public void test_with_keyvalues() {
        AccessLogEntry entry = newAccessLogEntry("test");
        entry.addKeyValue("singlevalue", "value1");
        entry.addKeyValue("multivalue", "value2");
        entry.addKeyValue("multivalue", "value3");

        String expectedOutput =
            "{\"ip\":\"152.200.54.243\"," +
            "\"time\":920880005.023," +
            "\"duration\":0.122," +
            "\"responsesize\":9875," +
            "\"code\":200," +
            "\"method\":\"GET\"," +
            "\"uri\":\"?query=test\"," +
            "\"version\":\"HTTP/1.1\"," +
            "\"agent\":\"Mozilla/4.05 [en] (Win95; I)\"," +
            "\"host\":\"localhost\"," +
            "\"scheme\":null," +
            "\"localport\":0," +
            "\"search\":{" +
            "\"totalhits\":1234," +
            "\"hits\":0," +
            "\"coverage\":{\"coverage\":100,\"documents\":100}" +
            "}," +
            "\"attributes\":{" +
            "\"singlevalue\":\"value1\"," +
            "\"multivalue\":[\"value2\",\"value3\"]}" +
            "}";

        assertEquals(expectedOutput, new JSONFormatter(entry).format());

    }

    @Test
    public void test_with_remoteaddrport() throws Exception {
        AccessLogEntry entry = newAccessLogEntry("test");

        // First test with only remote address and not port set
        entry.setRemoteAddress("FE80:0000:0000:0000:0202:B3FF:FE1E:8329");

        String expectedOutput =
            "{\"ip\":\"152.200.54.243\"," +
            "\"time\":920880005.023," +
            "\"duration\":0.122," +
            "\"responsesize\":9875," +
            "\"code\":200," +
            "\"method\":\"GET\"," +
            "\"uri\":\"?query=test\"," +
            "\"version\":\"HTTP/1.1\"," +
            "\"agent\":\"Mozilla/4.05 [en] (Win95; I)\"," +
            "\"host\":\"localhost\"," +
            "\"scheme\":null," +
            "\"localport\":0," +
            "\"remoteaddr\":\"FE80:0000:0000:0000:0202:B3FF:FE1E:8329\"," +
            "\"search\":{" +
            "\"totalhits\":1234," +
            "\"hits\":0," +
            "\"coverage\":{\"coverage\":100,\"documents\":100}" +
            "}" +
            "}";

        assertEquals(expectedOutput, new JSONFormatter(entry).format());

        // Add remote port and verify
        entry.setRemotePort(1234);

        expectedOutput =
            "{\"ip\":\"152.200.54.243\"," +
            "\"time\":920880005.023," +
            "\"duration\":0.122," +
            "\"responsesize\":9875," +
            "\"code\":200," +
            "\"method\":\"GET\"," +
            "\"uri\":\"?query=test\"," +
            "\"version\":\"HTTP/1.1\"," +
            "\"agent\":\"Mozilla/4.05 [en] (Win95; I)\"," +
            "\"host\":\"localhost\"," +
            "\"scheme\":null," +
            "\"localport\":0," +
            "\"remoteaddr\":\"FE80:0000:0000:0000:0202:B3FF:FE1E:8329\"," +
            "\"remoteport\":1234," +
            "\"search\":{" +
            "\"totalhits\":1234," +
            "\"hits\":0," +
            "\"coverage\":{\"coverage\":100,\"documents\":100}" +
            "}" +
            "}";

        assertEquals(expectedOutput, new JSONFormatter(entry).format());
    }

    @Test
    public void test_remote_address_same_as_ip_address() throws Exception {
        AccessLogEntry entry = newAccessLogEntry("test");
        AccessLogEntry entrywithremote = newAccessLogEntry("test");
        entrywithremote.setRemoteAddress(entry.getIpV4Address());

        assertEquals(new JSONFormatter(entry).format(), new JSONFormatter(entrywithremote).format());
    }

    @Test
    public void test_useragent_with_quotes() {
        final AccessLogEntry entry = new AccessLogEntry();
        entry.setRawQuery("query=test");
        entry.setRawPath("");
        entry.setIpV4Address(ipAddress);
        entry.setHttpMethod("GET");
        entry.setHttpVersion("HTTP/1.1");
        entry.setUserAgent("Mozilla/4.05 [en] (Win95; I; \"Best Browser Ever\")");
        entry.setHitCounts(new HitCounts(0, 10, 1234, 0, 10, new Coverage(100,200,200,0)));
        entry.setHostString("localhost");
        entry.setStatusCode(200);
        entry.setTimeStamp(920880005023L);
        entry.setDurationBetweenRequestResponse(122);
        entry.setReturnedContentSize(9875);

        String expectedOutput =
            "{\"ip\":\"152.200.54.243\"," +
            "\"time\":920880005.023," +
            "\"duration\":0.122," +
            "\"responsesize\":9875," +
            "\"code\":200," +
            "\"method\":\"GET\"," +
            "\"uri\":\"?query=test\"," +
            "\"version\":\"HTTP/1.1\"," +
            "\"agent\":\"Mozilla/4.05 [en] (Win95; I; \\\"Best Browser Ever\\\")\"," +
            "\"host\":\"localhost\"," +
            "\"scheme\":null," +
            "\"localport\":0," +
            "\"search\":{" +
            "\"totalhits\":1234," +
            "\"hits\":0," +
            "\"coverage\":{\"coverage\":50,\"documents\":100,\"degraded\":{\"non-ideal-state\":true}}" +
            "}" +
            "}";

        assertEquals(expectedOutput, new JSONFormatter(entry).format());
    }

    private void verifyCoverage(String coverage, AccessLogEntry entry) {
        assertEquals("{\"ip\":\"152.200.54.243\"," +
                "\"time\":920880005.023," +
                "\"duration\":0.122," +
                "\"responsesize\":9875," +
                "\"code\":200," +
                "\"method\":\"GET\"," +
                "\"uri\":\"?query=test\"," +
                "\"version\":\"HTTP/1.1\"," +
                "\"agent\":\"Mozilla/4.05 [en] (Win95; I)\"," +
                "\"host\":\"localhost\"," +
                "\"scheme\":null," +
                "\"localport\":0," +
                "\"search\":{" +
                "\"totalhits\":1234," +
                "\"hits\":0," +
                coverage +
                "}" +
                "}", new JSONFormatter(entry).format());
    }

    @Test
    public void test_with_coverage_degradation() {
        verifyCoverage("\"coverage\":{\"coverage\":50,\"documents\":100,\"degraded\":{\"non-ideal-state\":true}}",
                       newAccessLogEntry("test",  new Coverage(100,200,200,0)));
        verifyCoverage("\"coverage\":{\"coverage\":50,\"documents\":100,\"degraded\":{\"match-phase\":true}}",
                       newAccessLogEntry("test",  new Coverage(100,200,200,1)));
        verifyCoverage("\"coverage\":{\"coverage\":50,\"documents\":100,\"degraded\":{\"timeout\":true}}",
                       newAccessLogEntry("test",  new Coverage(100,200,200,2)));
        verifyCoverage("\"coverage\":{\"coverage\":50,\"documents\":100,\"degraded\":{\"adaptive-timeout\":true}}",
                       newAccessLogEntry("test",  new Coverage(100,200,200,4)));
    }
}
